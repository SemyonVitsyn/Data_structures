#pragma once

#include "igraph.hpp"


class SetGraph : public IGraph {

public:
    SetGraph(const size_t n_vertices = 0);
    SetGraph(const IGraph* graph);

    ~SetGraph() = default;

    void AddEdge(size_t from, size_t to) override;
    size_t VerticesCount() const noexcept override;
    void GetNextVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept override;
    void GetPrevVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept override;

    SetGraph(const SetGraph& ) = delete;
    SetGraph& operator=(const SetGraph& ) = delete;

private:
    std::vector<std::unordered_set<size_t>> graph_;

};