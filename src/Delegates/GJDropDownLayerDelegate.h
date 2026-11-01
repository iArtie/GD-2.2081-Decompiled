// GJDropDownLayerDelegate.h
#pragma once

class GJDropDownLayer;

class GJDropDownLayerDelegate {
public:
    virtual void dropDownLayerWillClose(GJDropDownLayer* layer) {}
    virtual ~GJDropDownLayerDelegate() {}
};