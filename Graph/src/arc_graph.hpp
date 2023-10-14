#pragma once

#include "igraph.hpp"


class ArcGraph : public IGraph {

public:
    ArcGraph(const size_t n_vertices = 0);
    ArcGraph(const IGraph* graph);

    ~ArcGraph() = default;

    void AddEdge(size_t from, size_t to) override;
    size_t VerticesCount() const noexcept override;
    void GetNextVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept override;
    void GetPrevVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept override;

    ArcGraph(const ArcGraph& ) = delete;
    ArcGraph& operator=(const ArcGraph& ) = delete;

private:
    std::vector<std::pair<size_t, size_t>> edges_;
    size_t vertices_count_ = 0;

};