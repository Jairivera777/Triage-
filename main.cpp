#include "backend.h"

int main() {
    httplib::Server svr;
    inicializarServidor(svr);
    svr.listen("0.0.0.0", 8080);
    return 0;
}
