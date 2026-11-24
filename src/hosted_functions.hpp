#pragma once

inline void host_print_u64(uint64_t v) { printf("%llu\n", v); fflush(stdout); }
inline void host_print_i32(int32_t v) { printf("%d\n", v);   fflush(stdout); }
static Param hp(ILTypeKind k) { return Param{ "", 0, ILType{k} }; }

struct HostFn {
    u32                id;
    std::string        name;
    ILType             ret;
    std::vector<Param> params;
    void* addr;
};

class HostTable {
    std::vector<HostFn> fns;
    std::unordered_map<std::string, u32> by_name;
public:
    HostTable() {
        add("print_u64", (void*)&host_print_u64, ILType{ ILTypeKind::Void }, { hp(ILTypeKind::U64) });
        add("print_i32", (void*)&host_print_i32, ILType{ ILTypeKind::Void }, { hp(ILTypeKind::I32) });
    }

    u32 add(std::string name, void* addr, ILType ret, std::vector<Param> params) {
        u32 id = (u32)fns.size();
        fns.push_back({ id, std::move(name), ret, std::move(params), addr });
        by_name[fns.back().name] = id;
        return id;
    }

    const HostFn& get(u32 id) {
        for (auto& fn : fns) {
            if (fn.id == id)
            {
                return fn;
            }
        }
        return fns[0];
    }

    const HostFn* find(const std::string& n) const {
        auto it = by_name.find(n);
        return it == by_name.end() ? nullptr : &fns[it->second];
    }
};