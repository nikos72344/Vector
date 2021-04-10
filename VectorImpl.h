//
// Created by mikita on 09.04.2021.
//

#ifndef VECTOR_VECTORIMPL_H
#define VECTOR_VECTORIMPL_H

#include "IVector.h"

class VectorImpl : public IVector {
private:
    static ILogger *LOGGER;
    size_t dim;

    static RC elemCheck(double elem);

    static size_t getMaxAbs(const double *data, const size_t dim);

    void doSum(VectorImpl *dest, double const *op1, double const *op2, bool doMinus = false);

    double doChebyshev() const;

    double doFirst() const;

    double doSecond() const;

    VectorImpl();

    VectorImpl(const VectorImpl &vector);

    VectorImpl &operator=(const VectorImpl &vector);

    VectorImpl(size_t dim);

public:
    static IVector *createVector(size_t dim, double const *const &ptr_data);

    static RC copyInstance(IVector *const dest, IVector const *const &src);

    static RC moveInstance(IVector *const dest, IVector *&src);

    IVector *clone() const;

    double const *getData() const;

    RC setData(size_t dim, double const *const &ptr_data);

    static RC setLogger(ILogger *const logger);

    RC getCord(size_t index, double &val) const;

    RC setCord(size_t index, double val);

    RC scale(double multiplier);

    size_t getDim() const;

    RC inc(IVector const *const &op);

    RC dec(IVector const *const &op);

    static IVector *add(IVector const *const &op1, IVector const *const &op2);

    static IVector *sub(IVector const *const &op1, IVector const *const &op2);

    static double dot(IVector const *const &op1, IVector const *const &op2);

    static bool equals(IVector const *const &op1, IVector const *const &op2, NORM n, double tol);

    double norm(NORM n) const;

    RC applyFunction(const std::function<double(double)> &fun);

    RC foreach(const std::function<void(double)> &fun) const;

    size_t sizeAllocated() const;

    ~VectorImpl() {};
};

#endif //VECTOR_VECTORIMPL_H
