#!/usr/bin/env python3
import urllib.request
import json
import base64

#URL = "https://mds3.fidoalliance.org/"
URL = "https://files.mcbeeland.ru/blob.jwt"

req = urllib.request.Request(
    URL
)

try:
    with urllib.request.urlopen(req) as response:
        jwt_data = response.read().decode('utf-8')
    
    # 2. JWT состоит из трех частей: Header.Payload.Signature
    # Нам нужен Payload (вторая часть), там лежит весь список устройств
    payload_b64 = jwt_data.split('.')[1]
    
    # Добавляем паддинг '=', если его не хватает для base64
    payload_b64 += '=' * (-len(payload_b64) % 4)
    
    # 3. Декодируем base64 в чистый JSON
    json_bytes = base64.urlsafe_b64decode(payload_b64)
    data = json.loads(json_bytes.decode('utf-8'))
    
    # Теперь в data['entries'] лежит массив ВСЕХ аутентификаторов в мире.
    # Давай отфильтруем их и запишем в C++ hpp-файл
    with open("include/aaguid_map.hpp", "w", encoding="utf-8") as f:
        f.write("// Generated file\n")
        f.write("#pragma once\n\n")
        f.write("#include <unordered_map>\n")
        f.write("#include <string_view>\n\n")
        f.write("inline const std::unordered_map<std::string_view, std::string_view> g_aaguid_map = {\n")
        
        for entry in data.get("entries", []):
            aaguid = entry.get("aaguid")
            # Нам нужны только те устройства, у которых есть AAGUID (Passkeys/FIDO2)
            if aaguid:
                metadata = entry.get("metadataStatement", {})
                description = metadata.get("description", "Unknown Device").replace('"', '\\"')
                f.write(f'    {{"{aaguid}", "{description}"}},\n')
                
        f.write("};\n")
        
    print("Файл aaguid_map.hpp успешно сгенерирован!")

except Exception as e:
    print(f"Ошибка: {e}")
