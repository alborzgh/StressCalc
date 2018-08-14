#ifndef SOILMAT_H
#define SOILMAT_H

#include<QString>
#include<QColor>

class soilLayer
{
public:
  soilLayer();
  soilLayer(QString lName, double nThick, double lUnitWeight, double lCohesion, double lFA,double lStiffness,QColor color);
   ~soilLayer();

  void setLayerName(QString name) {layerName = name;}
  void setLayerThickness(double H){layerH = H;}
  void setLayerCohesion(double c){layerC = c;}
  void setLayerFrictionAngle(double phi){layerPhi = phi;}
  void setLayerStiffness(double G){layerG = G;}
  void setLayerUnitWeight(double gamma){layerGamma = gamma;}
  void setLayerTopStress(double stress){topStress = stress;}
  void update();

  QString getLayerName(){ return layerName;}
  QColor getLayerColor(){return layerColor;}
  double getLayerThickness(){return layerH;}
  double getLayerCohesion(){return layerC;}
  double getLayerStiffness(){return layerG;}
  double getLayerFrictionAngle(){return layerPhi;}
  double getLayerUnitWeight(){return layerGamma;}
  double getLayerTopStress(){return topStress;}
  double getLayerBottomStress(){return botStress;}
  double getLayerTopStrength(){return topStrength;}
  double getLayerBottomStrength(){return botStrength;}

private:
  void calcStress();
  void calcStrength();

  QString layerName;
  double layerH;
  double layerGamma;
  double layerC;
  double layerPhi;
  double layerG;
  QColor layerColor;

double topStress;
double botStress;
double topStrength;
double botStrength;

static int numLayers;
};

#endif // SOILMAT_H
