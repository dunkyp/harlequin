#pragma once

#include "lut.hpp"
#include <memory>
#include <nanoflann.hpp>


template <typename T>
struct Grid
{
    struct Point
    {
        T L, a, b;
    };
    
    std::vector<Point> points;
    
    size_t kdtree_get_point_count() const { return points.size(); }
    
    T kdtree_get_point(const size_t index, const size_t dim) const {
        switch(dim) {
        case 0:
            return points[index].L;
        case 1:
            return points[index].a;
        case 2:
            return points[index].b;
        }
    }
};

template <typename T>
using KDTree = nanoflann::KDTreeSingleIndexAdaptor<nanoflann::L2_Simple_Adaptor<T, Grid<T>>,
                                                   Grid<T>,
                                                   3>;

class LabLut : public Lut {
public:
    LabLut();
    QRgb lookup(QRgb in) override;
private:
    std::unique_ptr<KDTree<double>> t;
};
