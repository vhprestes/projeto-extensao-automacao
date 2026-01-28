#include <IRremote.h>
#include <NewPing.h>

#define RECV_PIN 7
#define RELAY_LUZ 8
#define RELAY_VENT 9
#define TRIGGER_PIN 11
#define ECHO_PIN 12
#define MAX_DISTANCE 200

IRrecv irrecv(RECV_PIN);
decode_results results;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

boolean luz_estado = false;
boolean vent_estado = false;
unsigned long ultimo_movimento = 0;
const unsigned long TIMEOUT_LUZ = 30000; // 30 segundos

void setup() {
  Serial.begin(9600);
  
  pinMode(RELAY_LUZ, OUTPUT);
  pinMode(RELAY_VENT, OUTPUT);
  
  digitalWrite(RELAY_LUZ, HIGH);
  digitalWrite(RELAY_VENT, HIGH);
  
  irrecv.enableIRIn();
  
  Serial.println("Sistema de Automação Residencial Inicializado");
  Serial.println("Aguardando comandos...");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.print("Código recebido: ");
    Serial.println(results.value, HEX);
    
    processar_comando(results.value);
    
    irrecv.resume();
    delay(100);
  }
  
  unsigned int distancia = sonar.ping_cm();
  
  if (distancia > 0 && distancia < 50) {
    ultimo_movimento = millis();
    
    if (luz_estado == false) {
      digitalWrite(RELAY_LUZ, LOW);
      luz_estado = true;
      Serial.println("Movimento detectado - Luz ligada automaticamente");
    }
  }
  
  if (luz_estado && (millis() - ultimo_movimento > TIMEOUT_LUZ)) {
    digitalWrite(RELAY_LUZ, HIGH);
    luz_estado = false;
    Serial.println("Timeout - Luz desligada automaticamente");
  }
  
  delay(50);
}

void processar_comando(unsigned long codigo) {
  if (codigo == 0xFF30CF) {
    luz_estado = !luz_estado;
    digitalWrite(RELAY_LUZ, luz_estado ? LOW : HIGH);
    Serial.println(luz_estado ? "Luz LIGADA" : "Luz DESLIGADA");
  }
  
  else if (codigo == 0xFF18E7) {
    vent_estado = !vent_estado;
    digitalWrite(RELAY_VENT, vent_estado ? LOW : HIGH);
    Serial.println(vent_estado ? "Ventilador LIGADO" : "Ventilador DESLIGADO");
  }
}
