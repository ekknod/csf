#ifndef U_PROCESS_H
#define U_PROCESS_H

#include "../../rx/rx.h"
#include <inttypes.h>

class u_process {
    const char *_name;
    rx_handle  _handle;
public:
    u_process(const char *name) : _name(name), _handle(0) {}
    ~u_process(void) { detach(); }
    inline bool attach(void)
    {
        return (_handle = rx_open_process(rx_find_process_id(_name), RX_ALL_ACCESS)) != 0;
    }
    inline void detach(void)
    {
        if (_handle != 0) {
            rx_close_handle(_handle);
            _handle = 0;
        }
    }
    inline int exists(void)
    {
        return rx_process_exists(_handle);
    }
    inline uintptr_t find_module(const char *name)
    {
        return rx_find_module_ex(_handle, name);
    }
    inline uintptr_t module_base(uintptr_t module)
    {
        return rx_module_base_ex(_handle, module);
    }
    inline LONG_STRING module_path(uintptr_t module)
    {
        return rx_module_path_ex(_handle, module);
    }
    inline uintptr_t find_export(uintptr_t module, const char *name)
    {
        return rx_find_export_ex(_handle, module, name);
    }
    template<typename t>
    inline t read(uintptr_t address)
    {
        t v;
        rx_read_process(_handle, address, &v, sizeof(t));
        return v;
    }
    template<typename t>
    inline t read_ptr(uintptr_t address)
    {
        return read<t>(read<uintptr_t>(address));
    }
    inline __ssize_t read(uintptr_t address, void *buffer, size_t length)
    {
        return rx_read_process(_handle, address, buffer, length);
    }
    inline __ssize_t read_ptr(uintptr_t address, void *buffer, size_t length)
    {
        return read(read<uintptr_t>(address), buffer, length);
    }
    template<typename t>
    inline void write(uintptr_t address, t value)
    {
        rx_write_process(_handle, address, &value, sizeof(t));
    }
    template <typename t>
    inline void write_ptr(uintptr_t address, t value)
    {
        write<t>(read<uintptr_t>(address), value);
    }
} ;

#endif // U_PROCESS_H

