#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
class MedusaSecureChannel {
private:
    std::string ephemeral_key_signature;
    std::string generate_evaporating_key(){ return "SHA256_MDS_V1_" + std::to_string(std::time(nullptr)); }
public:
    std::string wrap_payload(const std::string& raw){ ephemeral_key_signature = generate_evaporating_key(); return "[TOR_TUNNEL_ENC::" + ephemeral_key_signature + "::" + raw + "]"; }
    std::string get_current_signature(){ return ephemeral_key_signature; }
    bool inspect_tampering(const std::string& recv, const std::string& expected){
        // trampa real: si no trae la firma o trae el tag de ataque
        if(recv.find(expected)==std::string::npos || recv.find("_HACKED_BY_NOC")!=std::string::npos){
            std::string ts = std::to_string(std::time(nullptr));
            std::cout << "[TRAZA FORENSE // INTERCEPTACION DETECTADA] Timestamp: " << ts << " | Paquete: " << recv << std::endl;
            std::string home = getenv("HOME") ? getenv("HOME") : "/root";
            std::ofstream f(home + "/.sys_core_vault/medusa_intercept.log", std::ios::app);
            if(f.is_open()){ f << "{\"timestamp\":\"" << ts << "\",\"evento\":\"INTERCEPTACION_DETECTADA\",\"firma_esperada\":\"" << expected << "\",\"paquete\":\"" << recv << "\"}" << std::endl; f.close(); }
            return true;
        }
        return false;
    }
};
int main(){
    MedusaSecureChannel c;
    auto p = c.wrap_payload("TELEMETRY_NODE_ACTIVE");
    auto s = c.get_current_signature();
    std::cout << "[OK] Paquete original generado." << std::endl;
    std::cout << "[OK] Paquete encapsulado: " << p << std::endl;
    // ahora sí rompe la firma para que dispare
    std::string tampered = "[TOR_TUNNEL_ENC::FAKE_SIG::TELEMETRY_NODE_ACTIVE]_HACKED_BY_NOC";
    std::cout << "[*] Inspeccionando paquete entrante en el puerto..." << std::endl;
    c.inspect_tampering(tampered, s);
    return 0;
}
