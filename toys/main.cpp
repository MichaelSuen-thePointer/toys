
#include <string>
#include <type_traits>

struct pod_struct
{
    char str[3];
    int a;
    int b;
};

struct struct_traits
{
    static void copy(pod_struct* dest, const pod_struct* src, size_t count)
    {
        memcpy(dest, src, sizeof(pod_struct) * count);
    }
    static void assign(pod_struct& dest, pod_struct& value)
    {
        dest = value;
    }
    static void assign(pod_struct* dest, size_t count, pod_struct& value)
    {
        for (size_t i = 0; i != count; i++)
        {
            dest[i] = value;
        }
    }
};

using struct_string = std::basic_string<pod_struct, struct_traits, std::allocator<pod_struct>>;

int main()
{
    struct_string ststr;

    ststr.push_back(pod_struct());
    ststr.push_back(pod_struct());
    ststr.find(pod_struct());
}