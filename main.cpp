#include "httplib.h"
#include <iostream>
#include <string>
#include <chrono>
using namespace std;
class Paciente{
    private:
        string nombre;
        string documento;
        string tipoDocumento;
        int edad;
        string sintomas;
        string motivo;
        int triage;
        string telefono;
        string correo;
        string direccion;
        time_t horaLlegada;
    public:
        Paciente(){}
        Paciente(string _sintomas, string _tipoDocumento, string _motivo, int _triage){
            nombre = "NA";
            tipoDocumento = _tipoDocumento;
            documento = "NA";
            edad = 0;
            sintomas = _sintomas;
            motivo = _motivo;
            triage = _triage;
            correo = "NA";
            telefono = "NA";
            direccion = "NA";
            auto ahora = chrono::system_clock::now();
            horaLlegada = chrono::system_clock::to_time_t(ahora);
        }
        Paciente(string _nombre, string _documento, string _tipoDocumento, int _edad, string _sintomas, string _motivo, int _triage, string _correo, string _telefono, string _direccion){
            nombre = _nombre;
            documento = _documento;
            tipoDocumento = _tipoDocumento;
            edad = _edad;
            sintomas = _sintomas;
            motivo = _motivo;
            triage = _triage;
            correo = _correo;
            telefono = _telefono;
            direccion = _direccion;
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
        AcudientePaciente(string _parentesco, Paciente _paciente){
            nombre = "NA";
            tipoDocumento = "NA";
            documento = "NA";
            parentesco = _parentesco;
            telefono = "NA";
            paciente = _paciente;
        }
};
int main(){
    cout << "Hola";
    return 0;
}