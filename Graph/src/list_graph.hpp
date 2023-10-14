#pragma once

#include "igraph.hpp"


class ListGraph : public IGraph {

public:
    ListGraph(const size_t n_vertices = 0);
    ListGraph(const IGraph* graph);

    ~ListGraph() = default;

    void AddEdge(size_t from, size_t to) override;
    size_t VerticesCount() const noexcept override;
    void GetNextVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept override;
    void GetPrevVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept override;

    ListGraph(const ListGraph& ) = delete;
    ListGraph& operator=(const ListGraph& ) = delete;

private:
    std::vector<std::vector<size_t>> graph_;

};