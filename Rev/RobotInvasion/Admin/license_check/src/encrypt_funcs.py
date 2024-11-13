import lief


def encrypt_function(binary, funcs, key):
    for func in funcs:
        offset = funcs[func][0]
        size = funcs[func][1]
        print(f"Encrypting {func} at {offset}")
        text_section = binary.get_section(".text")
        text_offset = offset - text_section.virtual_address
        text_section_file_offset = text_section.file_offset
        target_function_offset = text_section_file_offset + text_offset

        with open("license_check", "rb") as f:
            packed = list(f.read())
            for i in range(size):
                packed[target_function_offset + i] ^= key[i % len(key)]

        with open("license_check", "wb") as f:
            f.write(bytes(packed))

        print(f"Encrypted {func} at {offset}")


if __name__ == "__main__":
    init_list = ["_Z11custom_hashPKc", "_Z8seedRandj", "_Z19read_seed_from_filePKc", "_Z9init_randP9tagMTRandj", "_Z10xor_arraysPhPKhPKii", "_Z13compare_flagsPKcPKhi", "_Z12process_flagPKc", "_Z12generate_keyPhP9tagMTRandi"]

    key = [0x69, 0x79, 0x34, 0x23, 0x56, 0x23, 0x56, 0x35, 0x64, 0x45, 0x56, 0x34, 0x57, 0x73, 0x23, 0x23]
    key = bytes(key)

    functions = {}

    binary = lief.parse("license_check")

    for symbol in binary.symbols:
        if symbol.type == lief.ELF.SYMBOL_TYPES.FUNC:
            functions[symbol.name] = {
                    'address': symbol.value,
                    'size': symbol.size,
                    'visibility': str(symbol.visibility),
                    'binding': str(symbol.binding),
            }

    to_encrypt = {}

    for func in init_list:
        offset = functions[func]['address']
        size = functions[func]['size']
        print(f"Found {func} at {hex(offset)}, size {size}")
        to_encrypt[func] = [offset, size]

    encrypt_function(binary, to_encrypt, key)
