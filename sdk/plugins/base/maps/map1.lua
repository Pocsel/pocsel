
fullname = "My fucking map one"

is_default = true

equations = {
    my_perlin2 = {
        function_name = "perlin2",
        xn = 0.00130,
        xa = -0.0112,
        zn = 0.00139,
        za = 0.0761,
        mul = 123.5545,
        alpha = 1.7,
        beta = 2.0,
        n = 8,
        ip = 0,
        ipp = 7
    },
    perlin3_caves = {
        function_name = "perlin3",
        xn = 0.063,
        xa = -0.0937,
        yn = 0.142,
        ya = 0.0841,
        zn = 0.063,
        za = 0.0764,
        mul = 1.0,
        alpha = 2.0,
        beta = 2.0,
        n = 1,
        ip = 0,
        ipp = 7
    },
    perlin3_iron = {
        function_name = "perlin3",
        xn = 0.63,
        xa = -0.937,
        yn = 0.42,
        ya = 0.841,
        zn = 0.63,
        za = 0.764,
        mul = 1.0,
        alpha = 2.0,
        beta = 2.0,
        n = 1,
        ip = 0,
        ipp = 7
    },
    none = {
        function_name = "none"
    }
}

cave_value = 0.19

cubes = {
--    empty = {
--        priority = 0,
--        equation = "perlin3",
--        validator = {
--            name = "superior",
--            value = 0.19
--        }
--    },

--    water = {
--        priority = 0,
--        equation = "my_perlin2",
--        validator = {
--            name = "water_validator",
--            value = 28
--        }
--    },

    water = {
        {
            priority = 0,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "y_superior_to_eq",
                },
                {
                    equation = "none",
                    validator = "y_inferior_to",
                    value = 28
                }
            },
        },
        {
            priority = 0,
            validators = {
                {
                    equation = "perlin3_caves",
                    validator = "eq_superior_to",
                    value = cave_value
                },
                {
                    equation = "none",
                    validator = "y_inferior_to",
                    value = 28
                }
            }

        }
    },

    iron = {
        {
            priority = 2,
            validators = {
                {
                    equation = "perlin3_iron",
                    validator = "eq_superior",
                    value = 0.20
                },
                {
                    equation = "my_perlin2",
                    validator = "ay+b<eq",
                    a = 1,
                    b = 1
                },
                {
                    equation = "perlin3_caves",
                    validator = "eq_inferior_to",
                    value = cave_value
                }
            }
        }
    },
    rock = {
        {
            priority = 3,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "ay+b<eq",
                    a = 1,
                    b = 1
                },
                {
                    equation = "perlin3_caves",
                    validator = "eq_inferior_to",
                    value = cave_value
                }
            }
        }
    },
    grass = {
        {
            priority = 4,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "y_inferior_to_eq"
                },
                {
                    equation = "my_perlin2",
                    validator = "ay+b>eq",
                    a = 1,
                    b = 1.0 / 24.0
                },
                {
                    equation = "perlin3_caves",
                    validator = "eq_inferior_to",
                    value = cave_value
                }
            }
        }
    },
    dirt = {
        {
            priority = 5,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "y_inferior_to_eq"
                },
                {
                    equation = "perlin3_caves",
                    validator = "eq_inferior_to",
                    value = cave_value
                }
            }
        }
    }
}

