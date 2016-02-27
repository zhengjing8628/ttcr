//
//  Cell.h
//  ttcr
//
//  Created by Bernard Giroux on 16-02-27.
//  Copyright © 2016 Bernard Giroux. All rights reserved.
//

#ifndef Cell_h
#define Cell_h

#include <vector>

template <typename T, typename NODE, typename S>
class Cell {
public:
    Cell(const size_t n) : slowness(std::vector<T>(n)) { }
    
    const T getSlowness(const size_t n) const { return slowness[n]; }
    
    void setSlowness(const T s) {
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s;
        }
    }
    
    int setSlowness(const T *s, const size_t ns) {
        if ( slowness.size() != ns ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s[n];
        }
        return 0;
    }
    
    int setSlowness(const std::vector<T>& s) {
        if ( slowness.size() != s.size() ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s[n];
        }
        return 0;
    }
    
    T computeDt(const NODE& source, const S& node,
                const size_t cellNo) const {
        return slowness[cellNo] * source.getDistance( node );
    }
    
    T computeDt(const NODE& source, const NODE& node,
                const size_t cellNo) const {
        return slowness[cellNo] * source.getDistance( node );
    }
    
private:
    std::vector<T> slowness;
};


// Elliptical anisotropy in 2D (Y Dimension ignored)

template <typename T, typename NODE, typename S>
class CellElliptical {
public:
    CellElliptical(const size_t n) :
    slowness(std::vector<T>(n)),
    xi(std::vector<T>(n)) {
    }
    
    const T getSlowness(const size_t n) const { return slowness[n]; }
    
    void setSlowness(const T s) {
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s;
        }
    }
    
    int setSlowness(const T *s, const size_t ns) {
        if ( slowness.size() != ns ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s[n];
        }
        return 0;
    }
    
    int setSlowness(const std::vector<T>& s) {
        if ( slowness.size() != s.size() ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s[n];
        }
        return 0;
    }
    
    const T getXi(const size_t n) const { return xi[n]; }
    
    void setXi(const T s) {
        for ( size_t n=0; n<xi.size(); ++n ) {
            xi[n] = s;
        }
    }
    
    int setXi(const T *s, const size_t ns) {
        if ( xi.size() != ns ) {
            std::cerr << "Error: xi vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<xi.size(); ++n ) {
            xi[n] = s[n];
        }
        return 0;
    }
    
    int setXi(const std::vector<T>& s) {
        if ( xi.size() != s.size() ) {
            std::cerr << "Error: xi vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<xi.size(); ++n ) {
            xi[n] = s[n];
        }
        return 0;
    }
    
    T computeDt(const NODE& source, const S& node,
                const size_t cellNo) const {
        T lx = node.x - source.getX();
        T lz = node.z - source.getZ();
        return slowness[cellNo] * std::sqrt( lx*lx + xi[cellNo]*lz*lz );
    }
    
    T computeDt(const NODE& source, const NODE& node,
                const size_t cellNo) const {
        T lx = node.getX() - source.getX();
        T lz = node.getZ() - source.getZ();
        return slowness[cellNo] * std::sqrt( lx*lx + xi[cellNo]*lz*lz );
    }
    
private:
    std::vector<T> slowness;
    std::vector<T> xi;        // anisotropy ratio, xi = sz / sx, *** squared ***
    
};


// Elliptical anisotropy in 2D (Y Dimension ignored)

template <typename T, typename NODE, typename S>
class CellTiltedElliptical {
public:
    CellTiltedElliptical(const size_t n) :
    slowness(std::vector<T>(n)),
    aAngle(std::vector<T>(n)),
    ca(std::vector<T>(n)),
    sa(std::vector<T>(n)) {
    }
    
    const T getSlowness(const size_t n) const { return slowness[n]; }
    
    void setSlowness(const T s) {
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s;
        }
    }
    
    int setSlowness(const T *s, const size_t ns) {
        if ( slowness.size() != ns ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s[n];
        }
        return 0;
    }
    
    int setSlowness(const std::vector<T>& s) {
        if ( slowness.size() != s.size() ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s[n];
        }
        return 0;
    }
    
    const T getXi(const size_t n) const { return xi[n]; }
    
    void setXi(const T s) {
        for ( size_t n=0; n<xi.size(); ++n ) {
            xi[n] = s;
        }
    }
    
    int setXi(const T *s, const size_t ns) {
        if ( xi.size() != ns ) {
            std::cerr << "Error: xi vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<xi.size(); ++n ) {
            xi[n] = s[n];
        }
        return 0;
    }
    
    int setXi(const std::vector<T>& s) {
        if ( xi.size() != s.size() ) {
            std::cerr << "Error: xi vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<xi.size(); ++n ) {
            xi[n] = s[n];
        }
        return 0;
    }
    
    const T getAngle(const size_t n) const { return aAngle[n]; }
    
    void setAngle(const T s) {
        ca[0] = std::cos(s);
        sa[0] = std::sin(s);

        for ( size_t n=0; n<xi.size(); ++n ) {
            aAngle[n] = s;
            ca[n] = ca[0];
            sa[n] = sa[0];

        }
    }
    
    int setAngle(const T *s, const size_t ns) {
        if ( aAngle.size() != ns ) {
            std::cerr << "Error: angle vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<aAngle.size(); ++n ) {
            aAngle[n] = s[n];
            ca[n] = std::cos(s[n]);
            sa[n] = std::sin(s[n]);
        }
        return 0;
    }
    
    int setAngle(const std::vector<T>& s) {
        if ( aAngle.size() != s.size() ) {
            std::cerr << "Error: angle vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<aAngle.size(); ++n ) {
            aAngle[n] = s[n];
            ca[n] = std::cos(s[n]);
            sa[n] = std::sin(s[n]);
        }
        return 0;
    }
    
    T computeDt(const NODE& source, const S& node,
                const size_t cellNo) const {
        T lx = node.x - source.getX();
        T lz = node.z - source.getZ();
        T t1 = lx * ca[cellNo] + lz * sa[cellNo];
        T t2 = lz * ca[cellNo] - lx * sa[cellNo];
        
        return slowness[cellNo] * std::sqrt( t1*t1 + xi[cellNo]*xi[cellNo]*t2*t2 );
    }
    
    T computeDt(const NODE& source, const NODE& node,
                const size_t cellNo) const {
        T lx = node.getX() - source.getX();
        T lz = node.getZ() - source.getZ();
        T t1 = lx * ca[cellNo] + lz * sa[cellNo];
        T t2 = lz * ca[cellNo] - lx * sa[cellNo];
        
        return slowness[cellNo] * std::sqrt( t1*t1 + xi[cellNo]*xi[cellNo]*t2*t2 );
    }
    
private:
    std::vector<T> slowness;
    std::vector<T> xi;        // anisotropy ratio, xi = sz / sx, *** squared ***
    std::vector<T> aAngle;    // column-wise (z axis) anisotropy angle of the cells, in radians
    std::vector<T> ca;        // cosine of aAngle
    std::vector<T> sa;        // sine of aAngle
};




//  VTI anisotropy, P or SV phase, in 2D (Y Dimension ignored)
template <typename T, typename NODE, typename S>
class CellVTI_PSV {
public:
    CellVTI_PSV(const size_t n) :
    sign(1.0),
    Vp0(std::vector<T>(n)),
    Vs0(std::vector<T>(n)),
    epsilon(std::vector<T>(n)),
    delta(std::vector<T>(n)) {
    }
    
    const T getVp0(const size_t n) const { return Vp0[n]; }
    
    void setVp0(const T s) {
        for ( size_t n=0; n<Vp0.size(); ++n ) {
            Vp0[n] = s;
        }
    }
    
    int setVp0(const T *s, const size_t ns) {
        if ( Vp0.size() != ns ) {
            std::cerr << "Error: Vp0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vp0.size(); ++n ) {
            Vp0[n] = s[n];
        }
        return 0;
    }
    
    int setVp0(const std::vector<T>& s) {
        if ( Vp0.size() != s.size() ) {
            std::cerr << "Error: Vp0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vp0.size(); ++n ) {
            Vp0[n] = s[n];
        }
        return 0;
    }
    
    const T getVs0(const size_t n) const { return Vs0[n]; }
    
    void setVs0(const T s) {
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s;
        }
    }
    
    int setVs0(const T *s, const size_t ns) {
        if ( Vs0.size() != ns ) {
            std::cerr << "Error: Vs0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s[n];
        }
        return 0;
    }
    
    int setVs0(const std::vector<T>& s) {
        if ( Vs0.size() != s.size() ) {
            std::cerr << "Error: Vs0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s[n];
        }
        return 0;
    }
    
    const T getEpsilon(const size_t n) const { return epsilon[n]; }
    
    void setEpsilon(const T s) {
        for ( size_t n=0; n<epsilon.size(); ++n ) {
            epsilon[n] = s;
        }
    }
    
    int setEpsilon(const T *s, const size_t ns) {
        if ( epsilon.size() != ns ) {
            std::cerr << "Error: epsilon vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<epsilon.size(); ++n ) {
            epsilon[n] = s[n];
        }
        return 0;
    }
    
    int setEpsilon(const std::vector<T>& s) {
        if ( epsilon.size() != s.size() ) {
            std::cerr << "Error: epsilon vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<epsilon.size(); ++n ) {
            epsilon[n] = s[n];
        }
        return 0;
    }
    
    const T getDelta(const size_t n) const { return delta[n]; }
    
    void setDelta(const T s) {
        for ( size_t n=0; n<delta.size(); ++n ) {
            delta[n] = s;
        }
    }
    
    int setDelta(const T *s, const size_t ns) {
        if ( delta.size() != ns ) {
            std::cerr << "Error: delta vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<delta.size(); ++n ) {
            delta[n] = s[n];
        }
        return 0;
    }
    
    int setDelta(const std::vector<T>& s) {
        if ( delta.size() != s.size() ) {
            std::cerr << "Error: delta vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<delta.size(); ++n ) {
            delta[n] = s[n];
        }
        return 0;
    }
    

    T computeDt(const NODE& source, const S& node,
                const size_t cellNo) const {
        // theta: angle w/r to vertical axis
        T theta = atan2(node.x - source.getX(), node.z - source.getZ());
        T f = 1. - (Vs0[cellNo]*Vs0[cellNo]) / (Vp0[cellNo]*Vp0[cellNo]);
        
        T tmp = 1. + (2.*epsilon[cellNo]*sin(theta)*sin(theta)) / f;
        
        tmp = 1. + epsilon[cellNo]*sin(theta)*sin(theta) - f/2. +
        sign*f/2.*sqrt( tmp*tmp - (2.*(epsilon[cellNo]-delta[cellNo])*sin(2.*theta)*sin(2.*theta))/f );
        
        T v = Vp0[cellNo] * sqrt( tmp );
        return source.getDistance( node ) / v;
    }
    
    T computeDt(const NODE& source, const NODE& node,
                const size_t cellNo) const {
        // theta: angle w/r to vertical axis
        T theta = atan2(node.getX() - source.getX(), node.getZ() - source.getZ());
        T f = 1. - (Vs0[cellNo]*Vs0[cellNo]) / (Vp0[cellNo]*Vp0[cellNo]);
        
        T tmp = 1. + (2.*epsilon[cellNo]*sin(theta)*sin(theta)) / f;
        
        tmp = 1. + epsilon[cellNo]*sin(theta)*sin(theta) - f/2. +
        sign*f/2.*sqrt( tmp*tmp - (2.*(epsilon[cellNo]-delta[cellNo])*sin(2.*theta)*sin(2.*theta))/f );
        
        T v = Vp0[cellNo] * sqrt( tmp );
        return source.getDistance( node ) / v;
    }
    
    void setPhase(const int p) {
        if ( p==1 ) sign = 1.;  // P wave
        else sign = -1.;        // SV wave
    }
    
private:
    T sign;
    std::vector<T> Vp0;
    std::vector<T> Vs0;
    std::vector<T> epsilon;
    std::vector<T> delta;

};



//  VTI anisotropy, SH phase, in 2D (Y Dimension ignored)
template <typename T, typename NODE, typename S>
class CellVTI_SH {
public:
    CellVTI_SH(const size_t n) :
    Vs0(std::vector<T>(n)),
    gamma(std::vector<T>(n)) {
    }
    
    const T getVs0(const size_t n) const { return Vs0[n]; }
    
    void setVs0(const T s) {
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s;
        }
    }
    
    int setVs0(const T *s, const size_t ns) {
        if ( Vs0.size() != ns ) {
            std::cerr << "Error: Vs0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s[n];
        }
        return 0;
    }
    
    int setVs0(const std::vector<T>& s) {
        if ( Vs0.size() != s.size() ) {
            std::cerr << "Error: Vs0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s[n];
        }
        return 0;
    }
    
    const T getGamma(const size_t n) const { return gamma[n]; }
    
    void setGamma(const T s) {
        for ( size_t n=0; n<gamma.size(); ++n ) {
            gamma[n] = s;
        }
    }
    
    int setGamma(const T *s, const size_t ns) {
        if ( gamma.size() != ns ) {
            std::cerr << "Error: gamma vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<gamma.size(); ++n ) {
            gamma[n] = s[n];
        }
        return 0;
    }
    
    int setGamma(const std::vector<T>& s) {
        if ( gamma.size() != s.size() ) {
            std::cerr << "Error: gamma vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<gamma.size(); ++n ) {
            gamma[n] = s[n];
        }
        return 0;
    }
    
    T computeDt(const NODE& source, const S& node,
                const size_t cellNo) const {
        // theta: angle w/r to vertical axis
        T theta = atan2(node.x - source.getX(), node.z - source.getZ());
        T v = Vs0[cellNo] * sqrt(1. + 2.*gamma[cellNo]*sin(theta)*sin(theta));
        return source.getDistance( node ) / v;
    }
    
    T computeDt(const NODE& source, const NODE& node,
                const size_t cellNo) const {
        // theta: angle w/r to vertical axis
        T theta = atan2(node.getX() - source.getX(), node.getZ() - source.getZ());
        T v = Vs0[cellNo] * sqrt(1. + 2.*gamma[cellNo]*sin(theta)*sin(theta));
        return source.getDistance( node ) / v;
    }
    
private:
    std::vector<T> Vs0;
    std::vector<T> gamma;
    
};







// Elliptical anisotropy in 3D

template <typename T, typename NODE, typename S>
class CellElliptical3D {
public:
    CellElliptical3D(const size_t n) :
    slowness(std::vector<T>(n)),
    xi(std::vector<T>(n)) {
    }
    
    const T getSlowness(const size_t n) const { return slowness[n]; }
    
    void setSlowness(const T s) {
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s;
        }
    }
    
    int setSlowness(const T *s, const size_t ns) {
        if ( slowness.size() != ns ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s[n];
        }
        return 0;
    }
    
    int setSlowness(const std::vector<T>& s) {
        if ( slowness.size() != s.size() ) {
            std::cerr << "Error: slowness vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<slowness.size(); ++n ) {
            slowness[n] = s[n];
        }
        return 0;
    }
    
    T computeDt(const NODE& source, const S& node,
                const size_t cellNo) const {
        T lx = node.x - source.getX();
        T ly = node.y - source.getY();
        lx = std::sqrt( lx*lx + ly*ly ); // horizontal distance
        T lz = node.z - source.getZ();
        return slowness[cellNo] * std::sqrt( lx*lx + xi[cellNo]*lz*lz );
    }
    
    T computeDt(const NODE& source, const NODE& node,
                const size_t cellNo) const {
        T lx = node.getX() - source.getX();
        T ly = node.y - source.getY();
        lx = std::sqrt( lx*lx + ly*ly ); // horizontal distance
        T lz = node.getZ() - source.getZ();
        return slowness[cellNo] * std::sqrt( lx*lx + xi[cellNo]*lz*lz );
    }
    
private:
    std::vector<T> slowness;
    std::vector<T> xi;        // anisotropy ratio, xi = sz / sx, *** squared ***
    
};



//  VTI anisotropy, P or SV phase, in 2D (Y Dimension ignored)
template <typename T, typename NODE, typename S>
class CellVTI_PSV3D {
public:
    CellVTI_PSV3D(const size_t n) :
    sign(1.0),
    Vp0(std::vector<T>(n)),
    Vs0(std::vector<T>(n)),
    epsilon(std::vector<T>(n)),
    delta(std::vector<T>(n)) {
    }
    
    const T getVp0(const size_t n) const { return Vp0[n]; }
    
    void setVp0(const T s) {
        for ( size_t n=0; n<Vp0.size(); ++n ) {
            Vp0[n] = s;
        }
    }
    
    int setVp0(const T *s, const size_t ns) {
        if ( Vp0.size() != ns ) {
            std::cerr << "Error: Vp0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vp0.size(); ++n ) {
            Vp0[n] = s[n];
        }
        return 0;
    }
    
    int setVp0(const std::vector<T>& s) {
        if ( Vp0.size() != s.size() ) {
            std::cerr << "Error: Vp0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vp0.size(); ++n ) {
            Vp0[n] = s[n];
        }
        return 0;
    }
    
    const T getVs0(const size_t n) const { return Vs0[n]; }
    
    void setVs0(const T s) {
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s;
        }
    }
    
    int setVs0(const T *s, const size_t ns) {
        if ( Vs0.size() != ns ) {
            std::cerr << "Error: Vs0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s[n];
        }
        return 0;
    }
    
    int setVs0(const std::vector<T>& s) {
        if ( Vs0.size() != s.size() ) {
            std::cerr << "Error: Vs0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s[n];
        }
        return 0;
    }
    
    const T getEpsilon(const size_t n) const { return epsilon[n]; }
    
    void setEpsilon(const T s) {
        for ( size_t n=0; n<epsilon.size(); ++n ) {
            epsilon[n] = s;
        }
    }
    
    int setEpsilon(const T *s, const size_t ns) {
        if ( epsilon.size() != ns ) {
            std::cerr << "Error: epsilon vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<epsilon.size(); ++n ) {
            epsilon[n] = s[n];
        }
        return 0;
    }
    
    int setEpsilon(const std::vector<T>& s) {
        if ( epsilon.size() != s.size() ) {
            std::cerr << "Error: epsilon vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<epsilon.size(); ++n ) {
            epsilon[n] = s[n];
        }
        return 0;
    }
    
    const T getDelta(const size_t n) const { return delta[n]; }
    
    void setDelta(const T s) {
        for ( size_t n=0; n<delta.size(); ++n ) {
            delta[n] = s;
        }
    }
    
    int setDelta(const T *s, const size_t ns) {
        if ( delta.size() != ns ) {
            std::cerr << "Error: delta vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<delta.size(); ++n ) {
            delta[n] = s[n];
        }
        return 0;
    }
    
    int setDelta(const std::vector<T>& s) {
        if ( delta.size() != s.size() ) {
            std::cerr << "Error: delta vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<delta.size(); ++n ) {
            delta[n] = s[n];
        }
        return 0;
    }
    
    
    T computeDt(const NODE& source, const S& node,
                const size_t cellNo) const {
        // theta: angle w/r to vertical axis
        T lx = node.x - source.getX();
        T ly = node.y - source.getY();
        lx = std::sqrt( lx*lx + ly*ly ); // horizontal distance
        T theta = atan2(lx, node.z - source.getZ());
        T f = 1. - (Vs0[cellNo]*Vs0[cellNo]) / (Vp0[cellNo]*Vp0[cellNo]);
        
        T tmp = 1. + (2.*epsilon[cellNo]*sin(theta)*sin(theta)) / f;
        
        tmp = 1. + epsilon[cellNo]*sin(theta)*sin(theta) - f/2. +
        sign*f/2.*sqrt( tmp*tmp - (2.*(epsilon[cellNo]-delta[cellNo])*sin(2.*theta)*sin(2.*theta))/f );
        
        T v = Vp0[cellNo] * sqrt( tmp );
        return source.getDistance( node ) / v;
    }
    
    T computeDt(const NODE& source, const NODE& node,
                const size_t cellNo) const {
        // theta: angle w/r to vertical axis
        T lx = node.x - source.getX();
        T ly = node.y - source.getY();
        lx = std::sqrt( lx*lx + ly*ly ); // horizontal distance
        T theta = atan2(lx, node.getZ() - source.getZ());
        T f = 1. - (Vs0[cellNo]*Vs0[cellNo]) / (Vp0[cellNo]*Vp0[cellNo]);
        
        T tmp = 1. + (2.*epsilon[cellNo]*sin(theta)*sin(theta)) / f;
        
        tmp = 1. + epsilon[cellNo]*sin(theta)*sin(theta) - f/2. +
        sign*f/2.*sqrt( tmp*tmp - (2.*(epsilon[cellNo]-delta[cellNo])*sin(2.*theta)*sin(2.*theta))/f );
        
        T v = Vp0[cellNo] * sqrt( tmp );
        return source.getDistance( node ) / v;
    }
    
    void setPhase(const int p) {
        if ( p==1 ) sign = 1.;  // P wave
        else sign = -1.;        // SV wave
    }
    
private:
    T sign;
    std::vector<T> Vp0;
    std::vector<T> Vs0;
    std::vector<T> epsilon;
    std::vector<T> delta;
    
};



//  VTI anisotropy, SH phase, in 2D (Y Dimension ignored)
template <typename T, typename NODE, typename S>
class CellVTI_SH3D {
public:
    CellVTI_SH3D(const size_t n) :
    Vs0(std::vector<T>(n)),
    gamma(std::vector<T>(n)) {
    }
    
    const T getVs0(const size_t n) const { return Vs0[n]; }
    
    void setVs0(const T s) {
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s;
        }
    }
    
    int setVs0(const T *s, const size_t ns) {
        if ( Vs0.size() != ns ) {
            std::cerr << "Error: Vs0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s[n];
        }
        return 0;
    }
    
    int setVs0(const std::vector<T>& s) {
        if ( Vs0.size() != s.size() ) {
            std::cerr << "Error: Vs0 vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<Vs0.size(); ++n ) {
            Vs0[n] = s[n];
        }
        return 0;
    }
    
    const T getGamma(const size_t n) const { return gamma[n]; }
    
    void setGamma(const T s) {
        for ( size_t n=0; n<gamma.size(); ++n ) {
            gamma[n] = s;
        }
    }
    
    int setGamma(const T *s, const size_t ns) {
        if ( gamma.size() != ns ) {
            std::cerr << "Error: gamma vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<gamma.size(); ++n ) {
            gamma[n] = s[n];
        }
        return 0;
    }
    
    int setGamma(const std::vector<T>& s) {
        if ( gamma.size() != s.size() ) {
            std::cerr << "Error: gamma vectors of incompatible size.";
            return 1;
        }
        for ( size_t n=0; n<gamma.size(); ++n ) {
            gamma[n] = s[n];
        }
        return 0;
    }
    
    T computeDt(const NODE& source, const S& node,
                const size_t cellNo) const {
        // theta: angle w/r to vertical axis
        T lx = node.x - source.getX();
        T ly = node.y - source.getY();
        lx = std::sqrt( lx*lx + ly*ly ); // horizontal distance
        T theta = atan2(lx, node.z - source.getZ());
        T v = Vs0[cellNo] * sqrt(1. + 2.*gamma[cellNo]*sin(theta)*sin(theta));
        return source.getDistance( node ) / v;
    }
    
    T computeDt(const NODE& source, const NODE& node,
                const size_t cellNo) const {
        // theta: angle w/r to vertical axis
        T lx = node.x - source.getX();
        T ly = node.y - source.getY();
        lx = std::sqrt( lx*lx + ly*ly ); // horizontal distance
        T theta = atan2(lx, node.getZ() - source.getZ());
        T v = Vs0[cellNo] * sqrt(1. + 2.*gamma[cellNo]*sin(theta)*sin(theta));
        return source.getDistance( node ) / v;
    }
    
private:
    std::vector<T> Vs0;
    std::vector<T> gamma;
    
};


#endif /* Cell_h */
