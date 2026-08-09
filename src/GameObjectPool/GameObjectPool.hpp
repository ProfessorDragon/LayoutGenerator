#pragma once

class PoolObject;

class GameObjectPool {
   protected:
    static std::vector<PoolObject> pool;

    static const float BLOCK_SHARES;

    static const float VERY_SMALL_SHARES;

   public:
    static void generatePool(std::unordered_map<std::string, uint32_t> customObjects);

   protected:
    static void addPad(int tags, float shares, int objectId);

    static void addRing(int tags, float shares, int objectId);

    static void addDashRing(int tags, float shares, int objectId);

    static void addGravityPortal(int objectId, int inverseState);

    static void addSizePortal(int objectId, int inverseState);

    static void addSpeedPortal(int objectId, int inverseState, float shares);

    static void addGamemodePortal(int objectId, int inverseState);

   public:
    static const PoolObject* fish(std::function<float(const PoolObject*)> filter);
};
