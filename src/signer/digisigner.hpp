#ifndef __FDOTOOL_SIGNER_DIGISIGNER_H
#define __FDOTOOL_SIGNER_DIGISIGNER_H

#include <QtCore/QObject>

class DigiSigner : public QObject {
Q_OBJECT

public:

    explicit DigiSigner(const QByteArray &inputData, QObject *parent = Q_NULLPTR);

    ~DigiSigner();

    const QByteArray &getInputData() const;

    const QByteArray &getOutputData() const;

    void sign();

private:
    QByteArray inputData;
    QByteArray outputData;
};

#endif
