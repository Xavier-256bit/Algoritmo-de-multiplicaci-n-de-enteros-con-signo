#include <iostream>
#include <bitset>
#include <cmath>

// Funcion para ensamblar un numero en formato IEEE 754 a partir de componentes
float ensamblarFloat(int signo, int exponente, unsigned int significando) {
    unsigned int bits = 0;

    bits |= (signo << 31);              // Insertar el signo
    bits |= ((exponente & 0xFF) << 23); // Insertar el exponente
    bits |= (significando & 0x7FFFFF);  // Insertar el significando (23 bits)

    return *(float *)&bits; // Interpretar los bits como un float
}

// Funcion para descomponer un numero en formato IEEE 754 en sus componentes
void descomponerFloat(float numero, int &signo, int &exponente, unsigned int &significando) {
    unsigned int bits = *(unsigned int *)&numero; // Interpretar los bits del float como un entero

    signo = (bits >> 31) & 1;                  // Extraer el signo
    exponente = (bits >> 23) & 0xFF;           // Extraer el exponente
    significando = bits & 0x7FFFFF;            // Extraer el significando (23 bits)

    if (exponente != 0) {                      // Si no es anormal, añadir el bit implicito
        significando |= (1 << 23);
    }
}

// Imprimir un numero flotante en sus componentes binarios
void imprimirBits(float valor) {
    int signo, exponente;
    unsigned int significando;
    descomponerFloat(valor, signo, exponente, significando);

    std::cout << "Signo: " << signo << std::endl;
    std::cout << "Exponente: " << std::bitset<8>(exponente) << std::endl;
    std::cout << "Significando: " << std::bitset<23>(significando) << std::endl;
}

// Division de numeros en formato IEEE 754
float divisionPuntoFlotante(float X, float Y) {
    if (Y == 0.0f) {
        return INFINITY; // Division por cero
    } else if (X == 0) {
        return 0.0f; // Si el numerador es 0
    }

    int signoX, exponenteX, signoY, exponenteY;
    unsigned int significandoX, significandoY;
    descomponerFloat(X, signoX, exponenteX, significandoX); // Descomponer X
    descomponerFloat(Y, signoY, exponenteY, significandoY); // Descomponer Y

    // Calcular los componentes del resultado
    int signoZ = signoX ^ signoY; // Signo del resultado
    int exponenteZ = (exponenteX - exponenteY) + 127; // Exponente ajustado con Biased

    if (exponenteZ >= 255) {
        std::cout << "Overflow del exponente." << std::endl;
        return INFINITY;
    } else if (exponenteZ <= 0) {
        std::cout << "Underflow del exponente." << std::endl;
        return 0.0f;
    }

    // Escalar los significandos para la division
    unsigned long long significandoEscalado = static_cast<unsigned long long>(significandoX) << 23;
    unsigned int significandoZ = significandoEscalado / significandoY;

    // Normalizar si el 25vo bit esta activado
    if (significandoZ & (1 << 24)) {
        significandoZ >>= 1; // Ajustar significando
        exponenteZ++;        // Incrementar exponente
    }

    // Eliminar el bit implicito antes de ensamblar
    significandoZ &= 0x7FFFFF;

    return ensamblarFloat(signoZ, exponenteZ, significandoZ);
}

int main() {
    float X, Y, Z;

    // Entrada de datos
    std::cout << "Ingrese el valor de X: ";
    std::cin >> X;
    std::cout << "Ingrese el valor de Y: ";
    std::cin >> Y;

    // Realizar la division
    Z = divisionPuntoFlotante(X, Y);

    // Imprimir el resultado
    std::cout << "Resultado de la division:" << std::endl;
    imprimirBits(Z);
    std::cout << "Resultado en decimal: " << Z << std::endl;

    // Division directa
    float resultadoDirecto = X / Y;
    std::cout << "\nDivision directa usando C++:" << std::endl;
    std::cout << "Resultado de la division directa en bits:" << std::endl;
    imprimirBits(resultadoDirecto);
    std::cout << "Resultado en decimal: " << resultadoDirecto << std::endl;

    return 0;
}
