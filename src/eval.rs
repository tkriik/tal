use ::rpds::List;

use ::env::Env;
use ::primitive::PrimitiveError;
use ::sx::{Sx, SxSymbol};

#[derive(Eq, PartialEq, Debug)]
pub enum EvalError {
    Undefined(SxSymbol),
    Redefine(SxSymbol),

    SpecialTooFewArgs(SxSymbol),
    SpecialTooManyArgs(SxSymbol),

    DefineBadSymbol(Sx),

    // TODO: better info
    BadArg(Sx),
    NotAFunction(Sx),

    PrimitiveTooFewArgs(&'static str, usize, usize),

    Unknown(Sx)
}

pub type EvalResult = Result<Sx, EvalError>;

pub fn eval(env: &mut Env, sx: &Sx) -> EvalResult {
    match sx {
        Sx::Nil | Sx::Boolean(_) | Sx::Integer(_) | Sx::String(_) | Sx::SxPrimitive(_) => {
            return Ok(sx.clone());
        },

        Sx::List(l) if l.is_empty() => {
            return Ok(sx.clone());
        },

        Sx::Quote(v) => {
            return Ok(v.as_ref().clone());
        },

        Sx::Symbol(symbol) => {
            match env.lookup(symbol) {
                Some(v) => {
                    return Ok(v.clone());
                },

                None => {
                    return Err(EvalError::Undefined(symbol.clone()));
                }
            }
        },

        Sx::List(l) => {
            match (l.first(), l.drop_first()) {
                (Some(Sx::Symbol(ref symbol)), Some(ref args)) => {
                    match symbol.as_str() {
                        "def" => {
                            return apply_special(symbol, special_def, 2, env, &args);
                        },

                        "if" => {
                            return apply_special(symbol, special_if, 3, env, &args);
                        },

                        "quote" => {
                            return apply_special(symbol, special_quote, 1, env, &args);
                        },

                        _ => {
                            let head = Sx::Symbol(symbol.clone());
                            return apply(&head, env, &args);
                        }
                    }
                }

                _ => {
                    return Err(EvalError::Unknown(sx.clone()));
                }
            }
        }
    }
}

fn apply(head: &Sx, env: &mut Env, arglist: &List<Sx>) -> EvalResult {
    match eval(env, head) {
        Ok(Sx::SxPrimitive(primitive)) => {
            let mut args = Vec::new();
            for arg in arglist.iter() {
                match eval(env, arg) {
                    Ok(result) => {
                        args.push(result);
                    },

                    error @ Err(_) => {
                        return error;
                    }
                }
            }

            if args.len() < primitive.min_arity {
                return Err(EvalError::PrimitiveTooFewArgs(primitive.name, primitive.min_arity, args.len()));
            }

            let callback = primitive.callback;
            match callback(&args) {
                Ok(result) => {
                    return Ok(result);
                },

                Err(PrimitiveError::BadArg) => {
                    return Err(EvalError::BadArg(head.clone()));
                }
            }
        },

        Ok(_) => {
            return Err(EvalError::NotAFunction(head.clone()));
        }

        error @ Err(_) => {
            return error;
        }
    }
}

type SpecialFn = fn(&mut Env, &Vec<&Sx>) -> EvalResult;

fn apply_special(symbol: &SxSymbol,
                 special_fn: SpecialFn,
                 arity: usize,
                 env: &mut Env,
                 arglist: &List<Sx>) -> EvalResult {
    let mut args = Vec::new();
    for arg in arglist.iter() {
        args.push(arg);
    }

    if args.len() < arity {
        return Err(EvalError::SpecialTooFewArgs(symbol.clone()));
    }

    if arity < args.len() {
        return Err(EvalError::SpecialTooManyArgs(symbol.clone()));
    }

    return special_fn(env, &args);
}

fn special_def(env: &mut Env, args: &Vec<&Sx>) -> EvalResult {
    let binding = args[0];
    match binding {
        Sx::Symbol(symbol) => {
            match env.lookup(symbol) {
                None => {
                    let value = args[1];
                    match eval(env, value) {
                        Ok(result) => {
                            env.define(symbol, &result);
                            return Ok(binding.clone());
                        },

                        error @ Err(_) => {
                            return error;
                        }
                    }
                },

                Some(_) => {
                    return Err(EvalError::Redefine(symbol.clone()));
                }
            }
        },

        _ => {
            return Err(EvalError::DefineBadSymbol(binding.clone()));
        }
    }
}

fn special_if(env: &mut Env, args: &Vec<&Sx>) -> EvalResult {
    let cond = args[0];
    let true_path = args[1];
    let false_path = args[2];

    match eval(env, cond) {
        Ok(Sx::Nil) | Ok(Sx::Boolean(false)) => {
            return eval(env, false_path);
        },

        Ok(_) => {
            return eval(env, true_path);
        },

        error @ Err(_) => {
            return error;
        }
    }
}

fn special_quote(_env: &mut Env, args: &Vec<&Sx>) -> EvalResult {
    return Ok(args[0].clone());
}

#[cfg(test)]
mod tests {
    use super::*;

    use std::sync::Arc;

    use ::read::read;

    fn test_eval(input_source: &str, output_source: &str) {
        let mut env = Env::new();

        let input = read(input_source).expect("invalid input source");
        let output = read(output_source).expect("invalid output source");

        match input {
            Sx::List(sxs) => {
                let mut results = List::new();
                for sx in sxs.iter() {
                    results.push_front_mut(eval(&mut env, &sx).expect("eval error"));
                }
                results.reverse_mut();

                assert_eq!(Sx::List(Arc::new(results)).to_string(), output.to_string());
            },

            _ => {
                assert!(false);
            }
        }
    }

    fn test_eval_results(input_source: &str, exp_results: Vec<EvalResult>) {
        let mut env = Env::new();

        let input = read(input_source).expect("invalid input source");
        match input {
            Sx::List(sxs) => {
                let mut results = Vec::new();
                for sx in sxs.iter() {
                    results.push(eval(&mut env, &sx));
                }

                assert_eq!(results, exp_results);
            },

            _ => {
                assert!(false);
            }
        }
    }

    #[test]
    fn test_self_eval() {
        test_eval("nil", "nil");
        test_eval("true", "true");
        test_eval("false", "false");
        test_eval("0", "0");
        test_eval("-999", "-999");
        test_eval("\"Yellow submarine\"", "\"Yellow submarine\"");
        test_eval("\"北京市\"", "\"北京市\"");
        test_eval("()", "()");
    }

    #[test]
    fn test_single_quoted() {
        test_eval("'nil", "nil");
        test_eval("'true", "true");
        test_eval("'false", "false");
        test_eval("'0", "0");
        test_eval("'-999", "-999");
        test_eval("'\"Yellow submarine\"", "\"Yellow submarine\"");
        test_eval("'\"北京市\"", "\"北京市\"");
        test_eval("'()", "()");
        test_eval("'(1 2 3)", "(1 2 3)");
        test_eval("'foo", "foo");
    }

    #[test]
    fn test_double_quoted() {
        test_eval("''nil", "'nil");
        test_eval("''true", "'true");
        test_eval("''false", "'false");
        test_eval("''0", "'0");
        test_eval("''-999", "'-999");
        test_eval("''\"Yellow submarine\"", "'\"Yellow submarine\"");
        test_eval("''\"北京市\"", "'\"北京市\"");
        test_eval("''()", "'()");
        test_eval("''(1 2 3)", "'(1 2 3)");
        test_eval("''foo", "'foo");
    }

    #[test]
    fn test_special_def() {
        test_eval(r#"
            (def foo 1)
            foo
        "#, r#"
            foo
            1
        "#);
    }

    #[test]
    fn test_special_error_def() {
        let def_symbol = Arc::new("def".to_string());
        let foo_symbol = Arc::new("foo".to_string());

        test_eval_results(r#"
            (def)
            (def foo)
            (def foo 1 2)
            (def "foo" 1)
            (def foo 1)
            (def foo 2)
        "#, vec![
            Err(EvalError::SpecialTooFewArgs(def_symbol.clone())),
            Err(EvalError::SpecialTooFewArgs(def_symbol.clone())),
            Err(EvalError::SpecialTooManyArgs(def_symbol.clone())),
            Err(EvalError::DefineBadSymbol(sx_string!("foo"))),
            Ok(sx_symbol!("foo")),
            Err(EvalError::Redefine(foo_symbol.clone()))
        ]);
    }

    #[test]
    fn test_special_if_direct() {
        test_eval(r#"
            (if true "happy" "sad")
            (if "yay!" "happy" "sad")
            (if false "happy" "sad")
            (if nil "happy" "sad")
        "#, r#"
            "happy"
            "happy"
            "sad"
            "sad"
        "#);
    }

    #[test]
    fn test_special_if_short_circuit() {
        test_eval(r#"
            (if true "happy" undefined-symbol)
            (if false undefined-symbol "sad")
        "#, r#"
            "happy"
            "sad"
        "#);
    }

    #[test]
    fn test_special_error_if() {
        test_eval_results(r#"
            (if foo "happy" "sad")
            (if true foo "sad")
            (if false "happy" foo)
        "#, vec![
            Err(EvalError::Undefined(sx_symbol_unwrapped!("foo"))),
            Err(EvalError::Undefined(sx_symbol_unwrapped!("foo"))),
            Err(EvalError::Undefined(sx_symbol_unwrapped!("foo")))
        ])
    }

    #[test]
    fn test_special_if_indirect() {
        test_eval(r#"
            (def is-happy? true)
            (def is-not-happy? false)
            (if is-happy? "happy" "sad")
            (if is-not-happy? "happy" "sad")
        "#, r#"
            is-happy?
            is-not-happy?
            "happy"
            "sad"
        "#);
    }

    #[test]
    fn test_special_quote() {
        test_eval(r#"
            (quote 1)
            (quote foo)
            (quote (1 2 3))
        "#, r#"
            1
            foo
            (1 2 3)
        "#);
    }

    #[test]
    fn test_primitive_plus() {
        test_eval(r#"
            (+)
            (+ 1)
            (+ 0 0)
            (+ -1 1)
            (+ 1 1)
            (+ 999 1)
            (+ (+ 1 1) (+ 1 1))
            (+ 1 2 3)
            (+ 1 2 (+ 1 2) 4)
        "#, r#"
            0
            1
            0
            0
            2
            1000
            4
            6
            10
        "#);
    }
}
