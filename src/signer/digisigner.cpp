#include "digisigner.hpp"

DigiSigner::DigiSigner(const QByteArray &inputData, QObject *parent) : QObject(parent) {
    this->inputData = inputData;
}

DigiSigner::~DigiSigner() {

}

const QByteArray &DigiSigner::getInputData() const {
    return inputData;
}

const QByteArray &DigiSigner::getOutputData() const {
    return outputData;
}

void DigiSigner::sign() {
    outputData = inputData;
}
