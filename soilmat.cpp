#include <soilmat.h>
#include <math.h>
#include <mainwindow.h>
#include "globals.h"

const double pi = atan(1.0) * 4.0;


soilLayer::soilLayer()
{
  layerName      = QString("New Soil Layer");
  layerH         = 1.0;
  layerG         = 2.0e5;
  layerPhi       = 30;
  layerCohesion  = 0.0;
  layerGamma     = 18.0;
  layerGammaSat  = 18.0;
  layerTopStress = 0.0;
  layerColor = QColor(100,100,100,100);

  // waterUnitWeight = 9.81;
  waterUnitWeight = GAMMA_WATER;

  // TODO: Add functionality for setting GWT
  layerGWT = 1000.0;
}

soilLayer::soilLayer(QString lName, double nThick, double lUnitWeight, double lSatUnitWeight, double lStiffness,
                     double lFrictionAng, double lCohesion, QColor color)
{
  layerName      = lName;
  layerDepth     = 0.0;
  layerH         = nThick;
  layerG         = lStiffness;
  layerPhi       = lFrictionAng;
  layerCohesion  = lCohesion;
  layerGamma     = lUnitWeight;
  layerGammaSat  = lSatUnitWeight;
  layerTopStress = 0.0;
  layerColor     = color;

  waterUnitWeight = GAMMA_WATER;

  // TODO: Add functionality for setting GWT
  layerGWT = 1000.0;
}

soilLayer::~soilLayer()
{

}

double soilLayer::getLayerTopStress()
{
  return getEffectiveStress(0);
}

double soilLayer::getLayerBottomStress()
{
  return getEffectiveStress(layerH);
}

double soilLayer::getEffectiveStress(double depth)
{
  if ((depth < 0) || (depth > layerH))
    return 0.0;

  if (layerGWT < 0)
    return depth * (layerGammaSat - waterUnitWeight) + layerTopStress;
  else if (layerGWT > depth)
    return depth * layerGamma + layerTopStress;
  else
    return layerGWT * layerGamma + (depth - layerGWT) * (layerGammaSat - waterUnitWeight) + layerTopStress;
}


double soilLayer::getLayerTopStrength()
{
  return getEffectiveStress(0) * tan(pi / 180.0 * layerPhi) + layerCohesion;
}

double soilLayer::getLayerBottomStrength()
{
  return getEffectiveStress(layerH) * tan(pi / 180.0 * layerPhi) + layerCohesion;
}
