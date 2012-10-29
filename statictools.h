#ifndef STATICTOOLS_H
#define STATICTOOLS_H


//forward declaration
class IClientRealTimeSettings;

class StaticTools
{
public:
    static IClientRealTimeSettings *getClientRealTimeSettings();
};

#endif // STATICTOOLS_H
