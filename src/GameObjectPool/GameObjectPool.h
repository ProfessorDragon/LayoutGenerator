#pragma once

class PoolObject;

class GameObjectPool
{
protected:
    static const std::vector<PoolObject> POOL;

public:
    static const PoolObject *fish(std::function<float(const PoolObject *)> filter);
};
