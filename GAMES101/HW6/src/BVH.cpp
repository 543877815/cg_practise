#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    root = recursiveBuild(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}


int BVHAccel::computeSize(std::vector<Object*> objects) {
    int size = objects.size();
    Bounds3 bounds;
    for (int i = 0; i < size; ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    return bounds.SurfaceArea();
}

BVHBuildNode* BVHAccel::recursiveBuildSAH(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();
    int size = objects.size();

    // �������ڵ������õ����ڵ�İ�Χ��
    Bounds3 bounds;
    for (int i = 0; i < size; ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    // �����Ҷ�ӽڵ� ֱ�Ӵ洢������Ϣ ������
    if (size == 1)
    {
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    // ��������ݹ齨��
    else
    {
        // ��Ϊ���ڵ���������С��12ʱ �԰�ֵ�Ч�ʸ���
        if (size < 12)
        {
            auto l = objects.begin(), r = objects.end();
            auto mid = l + (size >> 1);
            auto u = std::vector<Object*>(l, mid), v = std::vector<Object*>(mid + 1, r);
            node->left = recursiveBuildSAH(u), node->right = recursiveBuildSAH(v);
            node->bounds = Union(node->left->bounds, node->right->bounds);
        }
        // �������SAH����
        else
        {
            int bestDiv = 0, bestDim = 0;
            float minCost = std::numeric_limits<float>::max();
            // ������ά��ͬ�Ļ���
            for (int dim = 0; dim < 3; dim++)
            {
                std::sort(objects.begin(), objects.end(), cmp[dim]);// �ֱ������ά����
                // ����Ͱ�ģ�ȷ���ֽ���
                const int binSize = 6;
                float div[binSize - 1] = { 1.0 / 6, 2.0 / 6, 3.0 / 6, 4.0 / 6, 5.0 / 6 };
                for (int i = 0; i < binSize - 1; i++) div[i] *= size;
                for (int i = 0; i < binSize - 1; i++)
                {
                    // ���ݹ�ʽ����cost
                    auto l = objects.begin(), r = objects.end(), mid = l + (size >> 1);
                    auto u = std::vector<Object*>(l, mid), v = std::vector<Object*>(mid, r);
                    auto uSize = computeSize(u), vSize = computeSize(v);
                    auto cost = 100.0 + (uSize * u.size() + vSize * u.size()) / bounds.SurfaceArea();
                    // ����ѡ��
                    if (cost < minCost)
                    {
                        minCost = cost;
                        bestDiv = div[i];
                        bestDim = dim;
                    }
                }
            }
            // ����ѡ����²�������
            if (bestDim != 2) std::sort(objects.begin(), objects.end(), cmp[bestDim]);
            auto l = objects.begin(), r = objects.end();
            auto mid = l + bestDiv;
            auto u = std::vector<Object*>(l, mid), v = std::vector<Object*>(mid, r);
            node->left = recursiveBuildSAH(u), node->right = recursiveBuildSAH(v);
            node->bounds = Union(node->left->bounds, node->right->bounds);
        }
        return node;
    }
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    // û�н��� ֱ�ӷ���
    if (!node->bounds.IntersectP(ray, ray.direction_inv)) return Intersection();
    // Ҷ�ӽ�����н��� ֱ�ӷ���
    if (node->left == nullptr && node->right == nullptr) return  node->object->getIntersection(ray);
    // ������
    auto u = getIntersection(node->left, ray), v = getIntersection(node->right, ray);
    return u.distance < v.distance ? u : v;// �����������
}