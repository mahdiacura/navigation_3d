#ifndef CCOORDINATE_H
#define CCOORDINATE_H

class CCoordinate{
public:
    double m_x;
    double m_y;
    double m_z;
    bool IsInitialized();

    void SetCoordinate  (double _x, double _y, double _z);
    CCoordinate         (double _x, double _y, double _z);
    CCoordinate         () = delete;
    ~CCoordinate        ();
private:
    bool m_isInitialized;
};

#endif
