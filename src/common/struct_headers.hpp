/*
 * File:   struct_headers.hpp
 * Author: markov
 */

#ifndef STRUCT_HEADERS_HPP
#define STRUCT_HEADERS_HPP

#include "project_defines.hpp"
#include "GnssLogTime.hpp"

typedef struct {
    unsigned char firstByte;
    unsigned char secondByte;
    unsigned char thirdByte;
    unsigned char headerLength;
    unsigned short messageID;
    unsigned char messageType;
    unsigned char portAddress;
    unsigned short messageLength;
    unsigned short sequence;
    unsigned char idleTime;
    unsigned char timeStatus;
    unsigned short week;
    unsigned int milliseconds;
    unsigned int receiverStatus;
    unsigned short reserved;
    unsigned short swVersion;
} LogRawHeader;

typedef struct {
    unsigned trackingState : 5;
    unsigned channelNumber : 5;
    unsigned isPhaseLock : 1;
    unsigned isParityKnown : 1;
    unsigned isCodeLocked : 1;
    unsigned correlatorType : 3;
    unsigned satelliteSystem : 3;
    unsigned reserved1 : 1;
    unsigned isGrouped : 1;
    unsigned signalType : 5;
    unsigned reserved2 : 1;
    unsigned isPrimary : 1;
    unsigned isHalfCycleAdded : 1;
    unsigned reserved3 : 1;
    unsigned isPRNLockedOut : 1;
    unsigned channelAssignment : 1;
}
__attribute__((packed)) LogRawTrackingStatus;

typedef struct {
    unsigned short satellitePRN;
    unsigned short glonassFrequency;
    double pseudorange;
    float pseudorangeDeviation;
    double carrierPhase;
    float carrierPhaseDeviation;
    float dopplerFrequency;
    float carrierToNoise;
    float lockTime;
    LogRawTrackingStatus channelTrackingStatus;
}
__attribute__((packed)) LogRawBodyRANGE;

typedef struct {
    unsigned char satellitePRN;
    signed char glonassFrequency;
    unsigned char satelliteSystem;
    unsigned char signalType;
    float azimuth;
    float elevation;
    float carrierToNoise;
    float lockTime;
    float averageCMC;
    float cmcStandardDeviation;
    float totalS4;
    float correctionS4;
    float phaseSigmaPhi1;
    float phaseSigmaPhi3;
    float phaseSigmaPhi10;
    float phaseSigmaPhi30;
    float phaseSigmaPhi60;
}
__attribute__((packed)) LogRawBodyISMREDOBS;

struct BinaryRawData {
#ifdef SMALL_RAW_DATA
    static const int MAX_DATA_SIZE = 4194304;
#else
    static const int MAX_DATA_SIZE = 33554432;
#endif

    unsigned char data[MAX_DATA_SIZE];
    size_t size;

    explicit BinaryRawData(unsigned char* newData, size_t newSize) {
        memcpy(data, newData, newSize);
        size = newSize;
    };

private:
    BinaryRawData() = delete;
    BinaryRawData(const BinaryRawData& orig) = delete;
};

typedef struct {
    signed long int lDeltaAdr : 20;
    unsigned long int lDeltaPower : 11;
    unsigned sign : 1;
}
__attribute__((packed)) LogRawOBSData;

typedef struct {
    unsigned char satellitePRN;
    signed char glonassFrequency;
    unsigned char signalType;
    unsigned char reserved;
    double firstADR;
    unsigned int firstPower;
    LogRawOBSData ismData[49];
}
__attribute__((packed)) LogRawBodyISMDETOBS;

typedef struct {
    double accumulatedDopplerRange;
    double power;
    unsigned int deltaMilliseconds;
} LogOBSData;

typedef struct {
    unsigned char satellitePRN;
    signed char glonassFrequency;
    unsigned char signalType;
    unsigned char satelliteSystem;
    LogOBSData ismData[50];
} LogBodyISMDETOBS;

typedef struct {
    unsigned bit1 : 1;
    unsigned pseudorangeCorrection : 3;
    unsigned reserved1 : 1;
    unsigned antennaWarning : 1;
    unsigned reserved2 : 2;
}
__attribute__((packed)) ExtendedSolutionStatus;

typedef struct {
    unsigned gallileoE1 : 1;
    unsigned reserved1 : 3;
    unsigned beidouB1 : 1;
    unsigned beidouB2 : 1;
    unsigned reserved2 : 2;
}
__attribute__((packed)) GalileoAndBeiDouMaskSignals;

typedef struct {
    unsigned gpsL1 : 1;
    unsigned gpsL2 : 1;
    unsigned gpsL5 : 1;
    unsigned reserved : 1;
    unsigned glonassL1 : 1;
    unsigned glonassL2 : 1;
    unsigned reserved2 : 2;
}
__attribute__((packed)) GPSAndGLONASSMaskSignals;

typedef struct {
    unsigned int solutionStatus;
    unsigned int positionType;
    double latitude;
    double longitude;
    double heightAboveSea;
    float undulation;
    unsigned int datumID;
    float latitudeDeviation;
    float longitudeDeviation;
    float heightDeviation;
    char baseStationID[4];
    float differentialAge;
    float solutionAge;
    unsigned char numberOfSatellitesTracked;
    unsigned char numberOfSatelliteVehicles;
    unsigned char reserved1;
    unsigned char reserved2;
    unsigned char reserved3;
    ExtendedSolutionStatus extendedSolutionStatus;
    GalileoAndBeiDouMaskSignals galileoAndBeiDouSignals;
    GPSAndGLONASSMaskSignals gpsAndGlonassMaskSignals;
}
__attribute__((packed)) LogRawBodyPSRPOS;

typedef struct {
    unsigned int solutionStatus;
    unsigned int positionType;
    double xCoordinate;
    double yCoordinate;
    double zCoordinate;
    float xDeviation;
    float yDeviation;
    float zDeviation;
    unsigned int velocityStatus;
    unsigned int velocityType;
    double xVelocity;
    double yVelocite;
    double zVelocity;
    float xVelocityDeviation;
    float yVelocityDeviation;
    float zVelocityDeviation;
    char baseStationID[4];
    float velocityLatency;
    float differentialAge;
    float solutionAge;
    unsigned char numberOfSatellitesTracked;
    unsigned char numberOfSatelliteVehicles;
    unsigned char reserved1;
    unsigned char reserved2;
    unsigned char reserved3;
    ExtendedSolutionStatus extendedSolutionStatus;
    GalileoAndBeiDouMaskSignals galileoAndBeiDouSignals;
    GPSAndGLONASSMaskSignals gpsAndGlonassMaskSignals;
}
__attribute__((packed)) LogRawBodyPSRXYZ;

typedef struct {
    //it will be blank, because it's not use
} LogRawBodyISMRAWOBS;

typedef struct {
    //it will be blank, because it's not use
} LogRawBodyISMRAWTEC;

typedef struct {
    unsigned int satelliteSystem;
    unsigned short satelliteID;
    short glonassFrequency;
    double xCoordinate;
    double yCoordinate;
    double zCoordinate;
    double SatelliteClockCorrection;
    double ionosphereDelay;
    double troposphereDelay;
    double reserved1;
    double reserved2;
}
__attribute__((packed)) LogRawBodySATXYZ2;

typedef struct {
    unsigned short satelliteID;
    short glonassFrequency;
    unsigned int satelliteHealth;
    double elevationAngle;
    double AzimuthAngle;
    double theoreticalDopplerOfSatellite;
    double ApparentDopplerForThisReceiver;
}
__attribute__((packed)) LogRawBodySATVIS2;

typedef struct {
    unsigned int satelliteSystem;
    bool isSatelliteVisibilityValid;
    bool isCompleteGNSSAlmanacUsed;
    unsigned short satelliteID;
    short glonassFrequency;
    unsigned int satelliteHealth;
    double elevationAngle;
    double azimuthAngle;
    double theoreticalDopplerOfSatellite;
    double ApparentDopplerForThisReceiver;
} LogBodySATVIS2;

#endif /* STRUCT_HEADERS_HPP */

