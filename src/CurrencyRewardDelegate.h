#pragma once

class CurrencyRewardLayer;

class CurrencyRewardDelegate {
public:
    virtual void currencyWillExit(CurrencyRewardLayer* layer);
    virtual ~CurrencyRewardDelegate() {}
};