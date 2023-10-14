#pragma once


class Complex {

public:

    Complex(double = 0.0, double = 0.0);
    Complex(Complex&);
    ~Complex();

    Complex& operator=(const Complex&);
    Complex& operator+=(const Complex&);
    Complex& operator-=(const Complex&);
    Complex& operator*=(const Complex&);

    Complex operator+(const Complex&) const;
    Complex operator-(const Complex&) const;
    Complex operator*(const Complex&) const;

    Complex operator+() const;
    Complex operator-() const;

    bool operator==(const Complex&) const;
    bool operator!=(const Complex&) const;

    double GetRe() const;
    double GetIm() const;

private:

    double Re, Im;
    
};
