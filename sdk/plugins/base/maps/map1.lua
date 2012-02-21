
fullname = "My fucking map one"

is_default = true

equations = {
    my_perlin2 = {
        function_name = "perlin2",
        xn = 0.015,
        xa = 0.0,
        zn = 0.015,
        za = 0.0,
        mul = 10.0,
        alpha = 1.7,
        beta = 2.0,
        n = 8,
        ip = 1,
        ipp = 2,
        ippmx = 1,
        ippmz = 1
    },
    perlin3_caves = {
        function_name = "perlin3",
        xn = 0.5,
        xa = 0.0,
        yn = 3.5,
        ya = 0.0,
        zn = 0.5,
        za = 0.0,
        mul = 1.0,
        alpha = 2.0,
        beta = 2.0,
        n = 2,
        ip = 1,
        ipp = 1,
        ippmx = 3,
        ippmz = 3,
        ippmy = 2
    },
    perlin3_iron = {
        function_name = "perlin3",
        xn = 2.0,
        xa = 0.0,
        yn = 45.0,
        ya = 0.0,
        zn = 2.0,
        za = 0.0,
        mul = 1.0,
        alpha = 2.0,
        beta = 2.0,
        n = 1,
        ip = 1,
        ipp = 1,
        ippmx = 2,
        ippmz = 2,
        ippmy = 1
    },
    none = {
        function_name = "none"
    }
}

cave_value = 0.22
height_value = 0

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

    void = {
        {
            priority = 0,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "y_superior_to_eq",
                },
                {
                    equation = "none",
                    validator = "y_superior_to",
                    value = height_value
                }
            }
        },
        {
            priority = 2,
            validators = {
                {
                    equation = "perlin3_caves",
                    validator = "eq_superior_to",
                    value = cave_value
                },
                {
                    equation = "none",
                    validator = "y_superior_to",
                    value = height_value
                }
            }

        }
    },

    water = {
        {
            priority = 1,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "y_superior_to_eq",
                }
            },
        },
        {
            priority = 3,
            validators = {
                {
                    equation = "perlin3_caves",
                    validator = "eq_superior_to",
                    value = cave_value
                }
            }

        }
    },
    grass = {
        {
            priority = 6,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "ay+b>eq",
                    a = 1,
                    b = 1.0 / 32.0
                },
                {
                    equation = "none",
                    validator = "y_superior_to",
                    value = height_value
                }
            }
        }
    },
    dirt = {
        {
            priority = 7,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "ay+b>eq",
                    a = 1,
                    b = 1
                }
            }
        }
    },
    iron = {
        {
            priority = 8,
            validators = {
                {
                    equation = "perlin3_iron",
                    validator = "ay+b<eq",
                    a=0.006,
                    b=0.60
                }
            }
        }
    },
    rock = {
        {
            priority = 9,
            validators = {
                {
                    equation = "my_perlin2",
                    validator = "ay+b<eq",
                    a = 1,
                    b = 1
                }
            }
        }
    }
}

