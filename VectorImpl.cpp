#include "VectorImpl.h"
#include <cmath>
#include <memory.h>

ILogger *VectorImpl::LOGGER = nullptr;

RC VectorImpl::setLogger(ILogger *const logger) {
    if (logger == nullptr)
        return RC::NULLPTR_ERROR;
    LOGGER = logger;
    return RC::SUCCESS;
}

RC VectorImpl::elemCheck(double elem) {
    if (std::isinf(elem)) {
        LOGGER->severe(RC::INFINITY_OVERFLOW, "VectorImpl.cpp", "elemCheck", 12);
        return RC::INFINITY_OVERFLOW;
    }
    if (std::isnan(elem)) {
        LOGGER->severe(RC::NOT_NUMBER, "VectorImpl.cpp", "elemCheck", 18);
        return RC::NOT_NUMBER;
    }
    return RC::SUCCESS;
}

VectorImpl::VectorImpl(size_t dim) {
    this->dim = dim;
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "VectorImpl", 29);
}

IVector *VectorImpl::createVector(size_t dim, const double *const &ptr_data) {
    if (LOGGER == nullptr)
        return nullptr;
    if (dim == 0 || ptr_data == nullptr) {
        LOGGER->severe(RC::NULLPTR_ERROR, "VectorImpl.cpp", "createVector", 37);
        return nullptr;
    }

    for (size_t i = 0; i < dim; i++)
        if (elemCheck(ptr_data[i]) != RC::SUCCESS)
            return nullptr;

    size_t size = sizeof(VectorImpl) + dim * sizeof(double);
    uint8_t *pInstance = new(std::nothrow) uint8_t[size];
    if (pInstance == nullptr) {
        LOGGER->severe(RC::ALLOCATION_ERROR, "VectorImpl.cpp", "createVector", 49);
        return nullptr;
    }

    uint8_t *pData = pInstance + sizeof(VectorImpl);
    memcpy(pData, (uint8_t *) ptr_data, dim * sizeof(double));
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "createVector", 44);
    return new(pInstance) VectorImpl(dim);
}

RC VectorImpl::copyInstance(IVector *const dest, const IVector *const &src) {
    if (dest == nullptr || src == nullptr) {
        LOGGER->warning(RC::NULLPTR_ERROR, "VectorImpl.cpp", "copyInstance", 62);
        return RC::NULLPTR_ERROR;
    }
    if (dest->sizeAllocated() < src->sizeAllocated()) {
        LOGGER->warning(RC::AMOUNT, "VectorImpl.cpp", "copyInstance", 67);
        return RC::AMOUNT;
    }
    if (abs((uint8_t *) src - (uint8_t *) dest) < dest->sizeAllocated()) {
        LOGGER->warning(RC::MEMORY_INTERSECTION, "VectorImpl.cpp", "copyInstance", 72);
        return RC::MEMORY_INTERSECTION;
    }

    dest->setData(src->getDim(), src->getData());

    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "copyInstance", 79);
    return RC::SUCCESS;
}

RC VectorImpl::moveInstance(IVector *const dest, IVector *&src) {
    RC res = copyInstance(dest, src);
    if (res != RC::SUCCESS)
        return res;
    delete src;
    src = nullptr;
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "moveInstance", 89);
    return RC::SUCCESS;
}

IVector *VectorImpl::clone() const {
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "clone", 94);
    return createVector(dim, getData());
}

double const *VectorImpl::getData() const {
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "getData", 99);
    return (double const *) ((uint8_t *) this + sizeof(VectorImpl));
}

RC VectorImpl::getCord(size_t index, double &val) const {
    if (index >= dim || index < 0) {
        LOGGER->warning(RC::INDEX_OUT_OF_BOUND, "VectorImpl.cpp", "getCord", 119);
        return RC::INDEX_OUT_OF_BOUND;
    }
    const double *data = getData();
    val = data[index];
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "getCord", 111);
    return RC::SUCCESS;
}

RC VectorImpl::setCord(size_t index, double val) {
    if (index >= dim || index < 0) {
        LOGGER->warning(RC::INDEX_OUT_OF_BOUND, "VectorImpl.cpp", "setCord", 118);
        return RC::INDEX_OUT_OF_BOUND;
    }
    RC temp = elemCheck(val);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "setCord", 123);
        return temp;
    }
    double *data = (double *) ((uint8_t *) this + sizeof(VectorImpl));
    data[index] = val;
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "setCord", 128);
    return RC::SUCCESS;
}

size_t VectorImpl::getMaxAbs(const double *data, const size_t dim) {
    size_t max = 0;
    for (size_t i = 0; i < dim; i++)
        if (max < fabs(data[i]))
            max = i;
    return max;
}

RC VectorImpl::scale(double multiplier) {
    RC temp = elemCheck(multiplier);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "scale", 143);
        return temp;
    }
    size_t max = getMaxAbs(getData(), dim);
    double *data = (double *) ((uint8_t *) this + sizeof(VectorImpl));
    temp = elemCheck(data[max] * multiplier);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "scale", 150);
        return temp;
    }
    for (size_t i = 0; i < dim; i++)
        data[i] *= multiplier;
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "scale", 155);
    return RC::SUCCESS;
}

size_t VectorImpl::getDim() const {
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "getDim", 160);
    return dim;
}

void VectorImpl::doSum(VectorImpl *dest, double const *op1, double const *op2, bool doMinus) {
    int s = doMinus ? -1 : 1;
    double *data = (double *) ((uint8_t *) dest + sizeof(VectorImpl));
    for (size_t i = 0; i < dest->dim; i++)
        data[i] = op1[i] + s * op2[i];
}

RC VectorImpl::inc(const IVector *const &op) {
    if (op == nullptr) {
        LOGGER->warning(RC::NULLPTR_ERROR, "VectorImpl.cpp", "inc", 174);
        return RC::NULLPTR_ERROR;
    }
    if (op->getDim() != dim) {
        LOGGER->warning(RC::MISMATCHING_DIMENSIONS, "VectorImpl.cpp", "inc", 179);
        return RC::MISMATCHING_DIMENSIONS;
    }

    size_t max = getMaxAbs(getData(), dim);
    RC temp = elemCheck(getData()[max] + op->getData()[max]);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "inc", 186);
        return temp;
    }

    max = getMaxAbs(op->getData(), op->getDim());
    temp = elemCheck(getData()[max] + op->getData()[max]);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "inc", 193);
        return temp;
    }

    doSum(this, getData(), op->getData());

    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "inc", 199);
    return RC::SUCCESS;
}

RC VectorImpl::dec(const IVector *const &op) {
    if (op == nullptr) {
        LOGGER->warning(RC::NULLPTR_ERROR, "VectorImpl.cpp", "dec", 186);
        return RC::NULLPTR_ERROR;
    }
    if (op->getDim() != dim) {
        LOGGER->warning(RC::MISMATCHING_DIMENSIONS, "VectorImpl.cpp", "dec", 191);
        return RC::MISMATCHING_DIMENSIONS;
    }

    size_t max = getMaxAbs(getData(), dim);
    RC temp = elemCheck(getData()[max] - op->getData()[max]);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "dec", 218);
        return temp;
    }

    max = getMaxAbs(op->getData(), op->getDim());
    temp = elemCheck(getData()[max] - op->getData()[max]);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "dec", 225);
        return temp;
    }

    doSum(this, getData(), op->getData(), true);

    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "dec", 231);
    return RC::SUCCESS;
}

IVector *VectorImpl::add(const IVector *const &op1, const IVector *const &op2) {
    if (op1 == nullptr || op2 == nullptr) {
        LOGGER->warning(RC::NULLPTR_ERROR, "VectorImpl.cpp", "add", 238);
        return nullptr;
    }
    size_t dim = op1->getDim();
    if (op2->getDim() != dim) {
        LOGGER->warning(RC::MISMATCHING_DIMENSIONS, "VectorImpl.cpp", "add", 244);
        return nullptr;
    }

    size_t max = getMaxAbs(op1->getData(), op1->getDim());
    RC temp = elemCheck(op1->getData()[max] + op2->getData()[max]);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "add", 251);
        return nullptr;
    }

    max = getMaxAbs(op2->getData(), op2->getDim());
    temp = elemCheck(op1->getData()[max] + op2->getData()[max]);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "add", 258);
        return nullptr;
    }

    IVector *newVector = createVector(dim, op1->getData());
    if (newVector == nullptr)
        return nullptr;

    temp = newVector->inc(op2);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "add", 268);
        delete newVector;
        return nullptr;
    }

    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "add", 273);
    return newVector;
}

IVector *VectorImpl::sub(const IVector *const &op1, const IVector *const &op2) {
    if (op1 == nullptr || op2 == nullptr) {
        LOGGER->warning(RC::NULLPTR_ERROR, "VectorImpl.cpp", "sub", 280);
        return nullptr;
    }
    size_t dim = op1->getDim();
    if (op2->getDim() != dim) {
        LOGGER->warning(RC::MISMATCHING_DIMENSIONS, "VectorImpl.cpp", "sub", 286);
        return nullptr;
    }

    size_t max = getMaxAbs(op1->getData(), op1->getDim());
    RC temp = elemCheck(op1->getData()[max] - op2->getData()[max]);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "sub", 293);
        return nullptr;
    }

    max = getMaxAbs(op2->getData(), op2->getDim());
    temp = elemCheck(op1->getData()[max] - op2->getData()[max]);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "sub", 300);
        return nullptr;
    }

    IVector *newVector = createVector(dim, op1->getData());
    if (newVector == nullptr)
        return nullptr;

    temp = newVector->dec(op2);
    if (temp != RC::SUCCESS) {
        LOGGER->warning(temp, "VectorImpl.cpp", "sub", 310);
        delete newVector;
        return nullptr;
    }

    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "sub", 315);
    return newVector;
}

double VectorImpl::dot(const IVector *const &op1, const IVector *const &op2) {
    if (op1 == nullptr || op2 == nullptr) {
        LOGGER->warning(RC::NULLPTR_ERROR, "VectorImpl.cpp", "dot", 322);
        return NAN;
    }
    size_t dim = op1->getDim();
    if (op2->getDim() != dim) {
        LOGGER->warning(RC::MISMATCHING_DIMENSIONS, "VectorImpl.cpp", "dot", 328);
        return NAN;
    }

    double res = 0;
    const double *one = op1->getData(), *two = op2->getData();
    for (size_t i = 0; i < dim; i++)
        res += one[i] * two[i];

    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "dot", 337);
    return res;
}

double VectorImpl::doChebyshev() const {
    const double *data = getData();
    double res = fabs(data[0]);
    for (size_t i = 0; i < dim; i++)
        if (res < fabs(data[i]))
            res = fabs(data[i]);
    return res;
}

double VectorImpl::doFirst() const {
    const double *data = getData();
    double res = 0;
    for (size_t i = 0; i < dim; i++)
        res += fabs(data[i]);
    return res;
}

double VectorImpl::doSecond() const {
    const double *data = getData();
    double res = 0;
    for (size_t i = 0; i < dim; i++)
        res += pow(data[i], 2);
    return sqrt(res);
}

double VectorImpl::norm(NORM n) const {
    double res = 0;
    switch (n) {
        case IVector::NORM::CHEBYSHEV:
            res = doChebyshev();
            break;
        case IVector::NORM::FIRST:
            res = doFirst();
            break;
        case IVector::NORM::SECOND:
            res = doSecond();
            break;
        case IVector::NORM::AMOUNT:
            res = NAN;
            break;
    }
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "norm", 373);
    return res;
}

bool VectorImpl::equals(const IVector *const &op1, const IVector *const &op2, NORM n, double tol) {
    IVector *temp = sub(op1, op2);
    if (temp == nullptr)
        return false;
    double res = temp->norm(n);
    delete temp;
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "equals", 383);
    return res <= tol;
}

RC VectorImpl::applyFunction(const std::function<double(double)> &fun) {
    double *data = (double *) ((uint8_t *) this + sizeof(VectorImpl));
    for (size_t i = 0; i < dim; i++)
        data[i] = fun(data[i]);
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "applyFunction", 391);
    return RC::SUCCESS;
}

RC VectorImpl::foreach(const std::function<void(double)> &fun) const {
    double *data = (double *) ((uint8_t *) this + sizeof(VectorImpl));
    for (size_t i = 0; i < dim; i++)
        fun(data[i]);
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "foreach", 399);
    return RC::SUCCESS;
}

size_t VectorImpl::sizeAllocated() const {
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "sizeAllocated", 404);
    return sizeof(VectorImpl) + dim * sizeof(double);
}

RC VectorImpl::setData(size_t dim, const double *const &ptr_data) {
    if (dim == 0 || this->dim < dim) {
        LOGGER->warning(RC::MISMATCHING_DIMENSIONS, "VectorImpl.cpp", "setData", 410);
        return RC::MISMATCHING_DIMENSIONS;
    }
    if (ptr_data == nullptr) {
        LOGGER->warning(RC::NULLPTR_ERROR, "VectorImpl.cpp", "setData", 414);
        return RC::NULLPTR_ERROR;
    }

    RC temp = RC::SUCCESS;
    for (size_t i = 0; i < dim; i++) {
        temp = elemCheck(ptr_data[i]);
        if (temp != RC::SUCCESS) {
            LOGGER->warning(temp, "VectorImpl.cpp", "setData", 422);
            return temp;
        }
    }

    double *data = (double *) ((uint8_t *) this + sizeof(VectorImpl));
    memcpy(data, ptr_data, dim * sizeof(double));
    this->dim = dim;
    LOGGER->info(RC::SUCCESS, "VectorImpl.cpp", "setData", 430);
    return RC::SUCCESS;
}