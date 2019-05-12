#include <iostream>
#include <fstream>
#include <io/bgeoexporter.h>

G_NAMESPACE_BEGIN

BgeoExporter::BgeoExporter() : m_frame(0), m_exportSize(5.f)
{
    m_exportPath = "./output/";

    // check endian.
    union {
        unsigned int t;
        char c[4];
    } b{ 0x01020304 };

    if (b.c[0] == 0x04)
        m_isBigEndian = false;
    else
        m_isBigEndian = true;
}

void BgeoExporter::exportParticles(const std::vector<Particle> &particles, int frame)
{
    // get current frame
    if (frame >= 0)
        m_frame = frame;

    std::string prefix = m_exportPath + std::to_string(m_frame);
    std::string filename = prefix + ".bgeo";
    // check if file exists
    while (true) {
        std::ifstream frameFile(filename);
        if (frameFile.good()) {
            // file exists, add some special characters
            prefix = prefix + "1";
            filename = prefix + ".bgeo";
            frameFile.close();
        }
        else {
            break;
        }
    }

    std::ofstream out;
    // now create the output file
    try {
        out.open(filename, std::ios::out | std::ios::binary);
        if (out.is_open()) {
            writeToFile(out, particles);
            std::cout << "An bgeo file is generated with frame:" << m_frame << std::endl;
            m_frame++;
        }
        else {
            std::cout << "Exporting frame " << m_frame << " couldn't open file." << std::endl;
        }
    }
    catch (std::ostream::failure e) {
        std::cout << "Exporting frame " << m_frame << " encountered IO error." << std::endl;
        return;
    }
}

bool BgeoExporter::writeToFile(std::ofstream &output, const std::vector<Particle> &particles)
{
    int magic = ((((('B' << 8) | 'g') << 8) | 'e') << 8) | 'o';
    char versionChar = 'V';
    int version = 5;
    int nPoints = particles.size();
    int nPrims = 0;
    int nPointGroups = 0;
    int nPrimGroups = 0;
    int nPointAttrib = 3;           // pos, mess, volume and velocity.           
    int nVertexAttrib = 0;
    int nPrimAttrib = 0;
    int nAttrib = 0;

    // total 41
    swapDataInPlace((char*)&magic, 4, 4);
    swapDataInPlace((char*)&version, 4, 4);
    swapDataInPlace((char*)&nPoints, 4, 4);
    swapDataInPlace((char*)&nPrims, 4, 4);
    swapDataInPlace((char*)&nPointGroups, 4, 4);
    swapDataInPlace((char*)&nPrimGroups, 4, 4);
    swapDataInPlace((char*)&nPointAttrib, 4, 4);
    swapDataInPlace((char*)&nVertexAttrib, 4, 4);
    swapDataInPlace((char*)&nPrimAttrib, 4, 4);
    swapDataInPlace((char*)&nAttrib, 4, 4);

    output.write((char*)&magic, 4)
        .write(&versionChar, 1)
        .write((char*)&version, 4)
        .write((char*)&nPoints, 4)
        .write((char*)&nPrims, 4)
        .write((char*)&nPointGroups, 4)
        .write((char*)&nPrimGroups, 4)
        .write((char*)&nPointAttrib, 4)
        .write((char*)&nVertexAttrib, 4)
        .write((char*)&nPrimAttrib, 4)
        .write((char*)&nAttrib, 4);

    // attributes total 38 + 6 + 8 + 5 = 57
    short size;
    int type;
    float def[4];
    writeHoudiniString(output, "mess");
    size = 1;
    type = 0;   // float
    def[0] = 1.f;
    swapDataInPlace((char*)&size, 2, 2);
    swapDataInPlace((char*)&type, 4, 4);
    swapDataInPlace((char*)def, 4, 4);
    output.write((char*)&size, 2).write((char*)&type, 4).write((char*)def, 4);
    writeHoudiniString(output, "volume");
    size = 1;
    type = 0;   // float
    def[0] = 1.f;
    swapDataInPlace((char*)&size, 2, 2);
    swapDataInPlace((char*)&type, 4, 4);
    swapDataInPlace((char*)def, 4, 4);
    output.write((char*)&size, 2).write((char*)&type, 4).write((char*)def, 4);
    writeHoudiniString(output, "vel");
    size = 3;
    type = 0;   // float
    def[0] = 0.f;
    def[1] = 0.f;
    def[2] = 0.f;
    swapDataInPlace((char*)&size, 2, 2);
    swapDataInPlace((char*)&type, 4, 4);
    swapDataInPlace((char*)def, 4, 12);
    output.write((char*)&size, 2).write((char*)&type, 4).write((char*)def, 12);

    // points
    for (Particle p : particles) {
        Vector3f pos = p.pos.cast<float>() * m_exportSize;
        def[0] = pos(0);
        def[1] = pos(1);
        def[2] = pos(2);
        def[3] = 1.f;
        float volume = 1.f;
        float mess = 1.f;
        Vector3f v = p.vel.cast<float>() * m_exportSize;
        float vel[3];
        vel[0] = v(0);
        vel[1] = v(1);
        vel[2] = v(2);
        swapDataInPlace((char*)def, 4, 16);
        swapDataInPlace((char*)&mess, 4, 4);
        swapDataInPlace((char*)&volume, 4, 4);
        swapDataInPlace((char*)vel, 4, 12);
        output.write((char*)def, 16);
        output.write((char*)&mess, 4);
        output.write((char*)&volume, 4);
        output.write((char*)vel, 12);
    }

    // write extra
    char extrabegin = 0x00;
    char extraend = 0xff;
    output.write(&extrabegin, 1);
    output.write(&extraend, 1);
    output.flush();
    return true;
}

void BgeoExporter::writeHoudiniString(std::ofstream &output, const std::string &str)
{
    short size = str.size();
    swapDataInPlace((char*)&size, 2, 2);
    const char *chs = str.c_str();
    output.write((char*)&size, 2);
    output.write(chs, str.size());
}

char* BgeoExporter::swapDataInPlace(char *data, int typesize, int size)
{
    if (m_isBigEndian)
        // only swap to big endian.
        return data;
    if (size < typesize)
        return data;     // error


    int cur = 0;
    char tmp;
    for (cur = 0; cur + typesize <= size; cur += typesize) {
        for (int i = 0; i < typesize - i; ++i) {
            tmp = data[cur + typesize - 1 - i];
            data[cur + typesize - 1 - i] = data[cur + i];
            data[cur + i] = tmp;
        }
    }

    return data;
}

G_NAMESPACE_END