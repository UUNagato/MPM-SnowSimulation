#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <common/math.h>
#include <mpm/particle.h>

//
// BGEO exporter. export particles to Houdini for snow rendering.
// currently, only points are supported. Geometries are not available.
// 
// Coded by XY. Li

G_NAMESPACE_BEGIN

class BgeoExporter
{
public:
    // BgeoExporter(const char* exportPath);
    BgeoExporter();
    
    void exportParticles(const std::vector<Particle> &particles, int frame = -1);


private:
    std::string m_exportPath;
    int m_frame;
    bool m_isBigEndian;
    float m_exportSize;

    bool writeToFile(std::ofstream &output, const std::vector<Particle> &particles);
    void writeHoudiniString(std::ofstream &output, const std::string &str);
    char* swapDataInPlace(char *data, int typesize, int size);
};

G_NAMESPACE_END