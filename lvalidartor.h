#ifndef LVALIDARTOR_H
#define LVALIDARTOR_H
#include <QValidator>


class lvalidator : public QValidator
    {
        Q_OBJECT
    public:
        explicit lvalidator(QObject *parent = 0);
        virtual State validate ( QString & input, int & pos ) const
        {
            if (input.isEmpty())
                return Acceptable;

            bool b;
            int val = input.toInt(&b);

            if ((b == true) && (val > 0) && (val < 7))
            {
                return Acceptable;
            }
            return Invalid;
        }
    };



#endif // LVALIDARTOR_H
