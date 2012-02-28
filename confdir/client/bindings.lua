-- Keys and mouse buttons bindings configuration
--
-- bind(key / button, action)
--
-- Possible key values (case insensitive):
--  - "a" -> "z", "F1" -> "F15", "0" -> "1", "Kp0" -> "Kp9"
--  - Any other single printable ASCII character
--  - "Backspace", "Tab", "Clear", "Return", "Pause", "Escape", "Space",
--    "Delete", "KpPeriod", "KpDivide", "KpMultiply", "KpMinus", "KpPlus",
--    "KpEnter", "KpEquals", "Up", "Down", "Right", "Left", "Insert", "Home",
--    "End", "Pageup", "Pagedown", "Numlock", "Capslock", "Scrollock",
--    "Rshift", "Lshift", "Rctrl", "Lctrl", "Ralt", "Lalt", "Rmeta", "Lmeta",
--    "Lsuper", "Rsuper", "Mode", "Compose", "Help", "Print", "Sysreq",
--    "Break", "Menu", "Power", "Euro", "Undo", "Lwindows", "Rwindows",
--    "AltGr"
--  - Any single UTF8 encoded character (not yet available!)
--
-- Possible button values (case insensitive):
--  "LeftClick", "MiddleClick", "RightClick", "WheelUp", "WheelDown",
--  "X1Click", "X2Click"
--
-- Note: actions are case sensitive.

bind("E", "forward")
bind("S", "left")
bind("D", "backward")
bind("F", "right")

bind("Up", "forward")
bind("Left", "left")
bind("Down", "backward")
bind("Right", "right")

bind("WheelDown", "jump")
bind("Space", "jump")

bind("Lctrl", "crouch")
bind("Rctrl", "crouch")

bind("LeftClick", "fire")
bind("RightClick", "altfire")

bind("Escape", "menu")

bind("W", "toggle-wireframe")

bind("T", "dump-octree")

bind("Q", "togglesprint")
