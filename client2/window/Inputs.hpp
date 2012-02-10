#ifndef __CLIENT_WINDOW_INPUTS_HPP__
#define __CLIENT_WINDOW_INPUTS_HPP__

namespace Client { namespace Window {

    namespace InputType {
        enum InputType
        {
            Pressed,
            Held,
            Released,
        };
    }

    namespace Button {
        enum Button
        {
            None,
            Left,
            Middle,
            Right,
            WheelUp,
            WheelDown,
            Special1,
            Special2,
        };
    }

    namespace SpecialKey {
        enum SpecialKey
        {
            None,
            Backspace,
            Tab,
            Clear,
            Return,
            Pause,
            Escape,
            Space,
            Delete,
            Kp0,
            Kp1,
            Kp2,
            Kp3,
            Kp4,
            Kp5,
            Kp6,
            Kp7,
            Kp8,
            Kp9,
            KpPeriod,
            KpDivide,
            KpMultiply,
            KpMinus,
            KpPlus,
            KpEnter,
            KpEquals,
            Up,
            Down,
            Right,
            Left,
            Insert,
            Home,
            End,
            Pageup,
            Pagedown,
            F1,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,
            F13,
            F14,
            F15,
            Numlock,
            Capslock,
            Scrollock,
            Rshift,
            Lshift,
            Rctrl,
            Lctrl,
            Ralt,
            Lalt,
            Rmeta,
            Lmeta,
            Lsuper,
            Rsuper,
            AltGr,
            Compose,
            Help,
            Print,
            Sysreq,
            Break,
            Menu,
            Power,
            Euro,
            Undo,
        };
    }

}}

#endif
