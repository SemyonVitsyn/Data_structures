#include "complex.hpp"


//-------------------------------------------------------------

Complex::Complex(double re, double im) {
    Re = re;
    Im = im;
}

Complex::Complex(Complex& obj) {
    Re = obj.Re;
    Im = obj.Im;
}

Complex::~Complex() = default;

//---------------------------assign----------------------------

Complex& Complex::operator=(const Complex& obj) {
    Re = obj.Re;
    Im = obj.Im;
    return *this;
}

Complex& Complex::operator+=(const Complex& obj) {
    Re += obj.Re;
    Im += obj.Im;
    return *this;
}

Complex& Complex::operator-=(const Complex& obj) {
    Re -= obj.Re;
    Im -= obj.Im;
    return *this;
}

Complex& Complex::operator*=(const Complex& obj) {
    double new_Re = Re * obj.Re - Im * obj.Im;
    double new_Im = Re * obj.Im + Im * obj.Re;
    Re = new_Re;
    Im = new_Im;
    return *this;
}

//-------------------------arithmetic--------------------------

Complex Complex::operator+(const Complex& obj) const {
    return Complex(Re + obj.Re, Im + obj.Im);
}

Complex Complex::operator-(const Complex& obj) const {
    return Complex(Re - obj.Re, Im - obj.Im);
}

Complex Complex::operator*(const Complex& obj) const {
    return Complex(Re * obj.Re - Im * obj.Im, Re * obj.Im + Im * obj.Re);
}

//--------------------------unary------------------------------

Complex Complex::operator+() const {
    return Complex(Re, Im);
}

Complex Complex::operator-() const {
    return Complex(-Re, -Im);
}

//-------------------------compare-----------------------------

bool Complex::operator==(const Complex& obj) const {
    return (Re == obj.Re && Im == obj.Im);
}    

bool Complex::operator!=(const Complex& obj) const {
    return !(this->operator==(obj));
}

//---------------------------get-------------------------------

double Complex::GetRe() const {
    return Re;
}

double Complex::GetIm() const {
    return Im;
}
