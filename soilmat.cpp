#include<soilmat.h>
#include<math.h>

int soilLayer::numLayers = 0;
const double pi = atan(1.0) * 4.0;


soilLayer::soilLayer()
{
  ++numLayers;
  layerName = QString("Layer" + QString::number(numLayers));
  layerH = 0;
  layerC = 0;
  layerPhi = 0;
  layerG = 0;
  layerGamma = 0;
  layerColor = QColor(100,100,100,100);
  topStrength = 0;
  topStress = 0;
  botStrength = 0;
  botStress = 0;
}

soilLayer::soilLayer(QString lName, double nThick, double lUnitWeight, double lCohesion, double lFA,double lStiffness, QColor color)
{
  ++numLayers;
  layerName = lName;
  layerH = nThick;
  layerC = lCohesion;
  layerPhi = lFA;
  layerG = lStiffness;
  layerGamma = lUnitWeight;
  layerColor = color;
  topStrength = 0;
  topStress = 0;
  botStrength = 0;
  botStress = 0;
}

soilLayer::~soilLayer()
{

}

void
soilLayer::update()
  {
    calcStress();
    calcStrength();
  }

void
soilLayer::calcStress()
{
  botStress = topStress + layerGamma * layerH;
}

void
soilLayer::calcStrength()
{
  topStrength = layerC + topStress * tan(layerPhi*pi/180);
  botStrength = layerC + botStress * tan(layerPhi*pi/180);
}
