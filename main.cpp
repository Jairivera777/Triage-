#include "httplib.h"
#include <iostream>
#include <string>
#include <chrono>
using namespace std;
class Paciente{ //clase que guarda los datos del paciente
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
        Paciente(string _sexo, string _tipoDocumento){
            nombre = "NA";
            tipoDocumento = _tipoDocumento;
            sexo = _sexo;
            documento = "NA";
            edad = 0;
            correo = "NA";
            telefono = "NA";
            direccion = "NA";
        }
        Paciente(string _nombre, string _documento, string _tipoDocumento, int _edad, string _sexo, string _eps, string _correo, string _telefono, string _direccion) {
            nombre = _nombre;
            documento = _documento;
            tipoDocumento = _tipoDocumento;
            edad = _edad;
            sexo = _sexo;
            eps = _eps;
            correo = _correo;
            telefono = _telefono;
            direccion = _direccion;
        }
        string getSexo(){
            return sexo;
        }
        int getEdad(){
            return edad;
        }
};
class AcudientePaciente{ //guarda los datos del acudiente si aplica
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
class Triage{ //clase que viene siendo donde se guardan los datos de la consulta
    private:
        Paciente paciente;
        bool embarazo;
        float temperatura;
        int estatura;
        float peso;
        int presionSistolica;
        int presionDiastolica;
        int frecuenciaCardiaca;
        int frecuenciaRespiratoria;
        int saturacion;
        int escala;
        int glasgow;
        string motivo;
        string alergias;
        string enfermedades;
        string cirugias;
        string observaciones;
        time_t horaLlegada;
        time_t tiempoEspera;
        int triage;
        int obtenerMinutosEspera(){ //nos dice cuanto tiempo ha estado esperando el paciente
            auto ahora = chrono::system_clock::now();
            time_t horaActual = chrono::system_clock::to_time_t(ahora);
            double segundos = difftime(horaActual, horaLlegada);
            return (int)(segundos/60);
        }
        bool buscarPalabras(string texto, string palabras[], int tamano){ //busca palabras
            for(int i = 0; i<tamano;i++){
                if(texto.find(palabras[i])!=string::npos){
                    return true;
                }
            }
            return false;
        }
        string normalizar(string palabra){ //Normaliza los textos para evitar errores
            for(char &c : palabra){
                c = toupper(c);
            }
            string textoLimpio = "";
            for(int i = 0; i < palabra.length(); i++){
                char letra = palabra[i];
                if(letra == 'Á') textoLimpio += 'A';
                else if(letra== 'É') textoLimpio += 'E';
                else if(letra=='Í') textoLimpio += 'I';
                else if(letra=='Ó') textoLimpio += 'O';
                else if(letra=='Ú'||letra=='Ü') textoLimpio += 'U';
                else textoLimpio += letra;
            }
            return textoLimpio;
        }
        int calcularTriage(){ //calcula el triage dependiendo del caso del paciente
            tiempoEspera = obtenerMinutosEspera();
            string texto = motivo+" "+observaciones;
            string palabrasNivel1[] = {"DISNEA DE REPOSO", "HABLA ENTRECORTADA", "USO MARCADO DE MUSCULOS ACCESORIOS", "CIANOSIS DISTAL", 
                "CIANOSIS PERIORAL", "ALTERACION DEL ESTADO DE ALERTA", "TRAUMA SEVERO", "POLITRAUMATISMO CRITICO", "PERDIDA DE CONSCIENCIA PROFUNDA", 
                "COMA", "ESTUPOR", "ACV", "ACCIDENTE CEREBRO VASCULAR", "HEMORRAGIA SEVERA", "QUEMADURAS EXTENSAS", "RIESGO DE PERDER UN MIEMBRO", 
                "RIESGO DE PERDER UN ORGANO", "SHOCK", "PERDIDA DE CONCIENCIA PROFUNDA", "PARO RESPIRATORIO", "APNEA", "INSUFICIENCIA RESPIRATORIA AGUDA",
                "PARO CARDIORESPIRATORIO", "PCR", "DOLOR TORACICO OPRESIVO", "INFARTO", "IAM", "HEMORRAGIA EXANGUINANTE", "CONVULSION", "ESTADO EPILEPTICO"};
            int tamano1 = sizeof(palabrasNivel1)/sizeof(palabrasNivel1[0]);    
            string palabrasNivel2[] = {"DOLOR MUY INTENSO", "DOLOR EN EL PECHO", "COLICOS NEFRITICOS", "INTOXICACION AGUDA", "ENVENENAMIENTO", "HEMORRAGIAS",
            "FIEBRE ALTA", "DIFICULTAD RESPIRATORIA MODERADA", "TRAUMATISMO", "LESIONES GRAVES"};
            int tamano2 = sizeof(palabrasNivel2)/sizeof(palabrasNivel2[0]);
            string epoc[] = {"EPOC", "ENFERMEDAD PULMONAR CRONICA"};
            bool encontroEpoc = buscarPalabras(observaciones, epoc, 2);
            string debilidad[] = {"DEBILIDAD EXTREMA", "CONFUSION"};
            string palabrasNivel3[] = {"DOLOR MODERADO", "VOMITO PERSISTENTE", "DIARREA RECUENTE", "DESHIDRATACION MODERADA", "HERIDA PARA SUTURA", 
                "SUTURA", "REACCION ALERGICA LEVE", "ESGUINCE", "SOSPECHA DE FRACTURA", "TRAUMA MODERADO", "LUXACION", "RETENCION URINARIA", 
                "DOLOR ABDOMINAL MODERADO", "CRISIS ASMATICA LEVE", "HEMOPTISIS LEVE", "SANGRE EN LA ORINA", "HEMATURIA","REACCION ADVERSA A MEDICAMENTO", 
                "HISTERIA", "CRISIS DE ANSIEDAD"};
            int tamano3 = sizeof(palabrasNivel3)/sizeof(palabrasNivel3[0]);
            string factoresRiesgo[] = {"DIABETES", "HIPERTENSION", "INSUFICIENCIA CARDIACA", "CANCER", "INMUNOSUPRESION"};
            string palabrasNivel4[] = {"DOLOR LEVE", "MOLESTIA LEVE", "DOLOR DE VARIOS DIAS", "RESFRIADO COMUN", "RESFRIADO","DOLOR DE GARGANTA", "CUADRO GRIPAL", 
                "GRIPA", "INFECCION URINARIA LEVE", "MOLESTIA AL ORINAR", "DIARREA SIMPLE", "DIARREA SIN DESHIDRATACION", "MOLESTIA DIGESTIVA", "CONTUSION LEVE",
                "GOLPE LEVE", "ESGUINCE MENOR", "HERIDA SUPERFICIAL", "RASPON", "ABSCESO PEQUEÑO", "INFECCION LOCALIZADA"};
            int tamano4 = sizeof(palabrasNivel4) / sizeof(palabrasNivel4[0]);
            int triage;
            if(frecuenciaCardiaca == 0 || buscarPalabras(texto, palabrasNivel1, tamano1) || saturacion < 80 || glasgow <= 8
                || (triage==2&&tiempoEspera>30) || (triage==3&&tiempoEspera>180 || (triage==4&&tiempoEspera>420))
                || (triage==5&&tiempoEspera>=720)) triage = 1;
            else if(presionSistolica>=180 || presionDiastolica>110 || presionSistolica<=90 || frecuenciaCardiaca>=120||
                frecuenciaCardiaca<=50 || frecuenciaRespiratoria>24 || frecuenciaRespiratoria<10 || (saturacion < 90 && !encontroEpoc) 
                ||(saturacion<=85&&encontroEpoc) || temperatura >=39.5 || (temperatura<=35&&buscarPalabras(texto, debilidad, 2)) 
                || buscarPalabras(texto, palabrasNivel2, tamano2) || temperatura <= 34) triage = 2;
            else if(temperatura>=38 || presionSistolica>=140 || frecuenciaCardiaca>=100 || frecuenciaRespiratoria>=21
            ||buscarPalabras(texto, palabrasNivel3, tamano3) || 
            (paciente.getEdad()>=65 && buscarPalabras(enfermedades, factoresRiesgo, 5))) triage = 3;
            else if(buscarPalabras(texto, palabrasNivel4, tamano4)) triage = 4;
            else triage = 5;
            return triage;
        }
    public:
        Triage(Paciente _paciente, bool _embarazo, float _temperatura, int _estatura, float _peso, int _presionSistolica, 
            int _presionDiastolica, int _frecuenciaCardiaca, int _frecuenciaRespiratoria, int _saturacion, int _escala, string _motivo, 
            string _alergias, string _enfermedades, string _cirugias, string _observaciones, int _glasgow){
            paciente = _paciente;
            if(paciente.getSexo()=="Masculino") embarazo = 0;
            else embarazo = _embarazo;
            temperatura = _temperatura;
            estatura = _estatura;
            peso = _peso;
            presionSistolica = _presionSistolica;
            presionDiastolica = _presionDiastolica;
            frecuenciaCardiaca = _frecuenciaCardiaca;
            frecuenciaRespiratoria = _frecuenciaRespiratoria;
            saturacion = _saturacion;
            escala = _escala;
            glasgow = _glasgow;
            motivo = normalizar(_motivo);
            alergias = normalizar(_alergias);
            enfermedades = normalizar(_enfermedades);
            cirugias = normalizar(_cirugias);
            observaciones = normalizar(_observaciones);
            auto ahora = chrono::system_clock::now();
            horaLlegada = chrono::system_clock::to_time_t(ahora);
            triage = calcularTriage();
        }
        int getTriage(){
            triage = calcularTriage();
            return triage;
        }
};
int main(){
    httplib::Server svr; //crea el servidor
    svr.listen("0.0.0.0", 8080); //le coloca una ip (local) y un puerto y lo manda por el chat de la consola
    return 0;
}