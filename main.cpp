#include "httplib.h"
#include <iostream>
#include <string>
#include <chrono>
using namespace std;
class Paciente{
    private:
        string nombre;
        string tipoDocumento;
        string sexo;
        string documento;
        int edad;
        string eps;
        string correo;
        string telefono;
        string direccion;
    public:
        Paciente(){}
        Paciente(string _sexo, string _tipoDocumento, string _motivo, int _triage){
            nombre = "NA";
            tipoDocumento = _tipoDocumento;
            sexo = _sexo;
            documento = "NA";
            edad = 0;
            correo = "NA";
            telefono = "NA";
            direccion = "NA";
        }
        string getSexo(){
            return sexo;
        }
};
class AcudientePaciente{
    private:
        string nombre;
        string tipoDocumento;
        string documento;
        string parentesco;
        string telefono;
        Paciente paciente;
    public:
        AcudientePaciente(string _nombre, string _tipoDocumento, string _documento, string _parentesco, string _telefono, Paciente _paciente){
            nombre = _nombre;
            tipoDocumento = _tipoDocumento;
            documento = _documento;
            parentesco = _parentesco;
            telefono = _telefono;
            paciente = _paciente;
        }
};
class Triage{
    private:
        Paciente paciente;
        bool embarazo;
        float temperatura;
        int estatura;
        float peso;
        int presion;
        int frecuencia;
        int saturacion;
        int escala;
        string motivo;
        string alergias;
        string enfermedades;
        string cirugias;
        string observaciones;
        time_t horaLlegada;
        int triage;
        int calcularTriage(){
            string palabrasNivel0[] = {"DISNEA DE REPOSO", "HABLA ENTRECORTADA", "USO MARCADO DE MUSCULOS ACCESORIOS", "CIANOSIS DITAL", "CIANOSIS PERIORAL", "ALTERACION DEL ESTADO DE ALERTA"};
            int triage;
            if(frecuencia==0 || buscarPalabras(observaciones, palabrasNivel0) || )
            return triage;
        }
        string mayusculas(string palabra){
            for(char &c : palabra){
                c = toupper(c);
            }
            return palabra;
        }
        bool buscarPalabras(string texto, string palabras[]){
            int tamano = sizeof(palabras)/sizeof(palabras[0]);
            for(int i = 0; i<tamano;i++){
                if(texto.find(palabras[i])!=string::npos){
                    return true;
                }
            }
            return false;
        }
    public:
        Triage(Paciente _paciente, bool _embarazo, float _temperatura, int _estatura, float _peso, int _presion, int _frecuencia, int _saturacion, int _escala, string _motivo, string _alergias, string _enfermedades, string _cirugias, string _observaciones){
            paciente = _paciente;
            if(paciente.getSexo()=="Masculino") embarazo = 0;
            else embarazo = _embarazo;
            temperatura = _temperatura;
            estatura = _estatura;
            peso = _peso;
            presion = _presion;
            frecuencia = _frecuencia;
            saturacion = _saturacion;
            escala = _escala;
            motivo = _motivo;
            alergias = mayusculas(_alergias);
            enfermedades = mayusculas(_enfermedades);
            cirugias = mayusculas(_cirugias);
            observaciones = mayusculas(_observaciones);
            auto ahora = chrono::system_clock::now();
            horaLlegada = chrono::system_clock::to_time_t(ahora);
            triage = calcularTriage();
        }
        int getTriage(){
            return triage;
        }
};
int main(){
    httplib::Server svr;
    svr.listen("0.0.0.0", 8080);
    return 0;
}