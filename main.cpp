#include <conio.h>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>

#include "devtools.hpp"

struct Node
{
    std::string node_id;
    std::string color {"black"};
    std::string fontcolor {"black"};
    double penwidth = 1.0;
};

struct Link
{
    int weight = 0;  // �� unsigned ����� �������� underflow
    std::string label {"0"};
    std::string color {"black"};
    std::string fontcolor {"black"};
    double penwidth = 1.0;
};

using UndirGraph = typename boost::adjacency_list< boost::vecS,
                                                   boost::vecS,
                                                   boost::undirectedS,
                                                   Node,
                                                   Link >;
using Vertex = typename boost::graph_traits< UndirGraph >::vertex_descriptor;
using Edge = typename boost::graph_traits< UndirGraph >::edge_descriptor;
using VertexIter = typename boost::graph_traits< UndirGraph >::vertex_iterator;
using EdgeIter = typename boost::graph_traits< UndirGraph >::edge_iterator;

int main()
{
    setlocale(LC_ALL, "Russian");

    // ��������� ������������ ������� �����
    UndirGraph graph {};
    boost::dynamic_properties dyn_prop
        {boost::ignore_other_properties};  // ���������� ���������� ��������

    auto node_id_map = boost::get(&Node::node_id, graph);
    dyn_prop.property("node_id", node_id_map);

    auto node_color_map = boost::get(&Node::color, graph);
    dyn_prop.property("color", node_color_map);

    auto node_fontcolor_map = boost::get(&Node::fontcolor, graph);
    dyn_prop.property("fontcolor", node_fontcolor_map);

    auto node_penwidth_map = boost::get(&Node::penwidth, graph);
    dyn_prop.property("penwidth", node_penwidth_map);

    auto link_weight_map = boost::get(&Link::weight, graph);
    dyn_prop.property("weight", link_weight_map);

    auto link_label_map = boost::get(&Link::label, graph);
    dyn_prop.property("label", link_label_map);

    auto link_color_map = boost::get(&Link::color, graph);
    dyn_prop.property("color", link_color_map);

    auto link_fontcolor_map = boost::get(&Link::fontcolor, graph);
    dyn_prop.property("fontcolor", link_fontcolor_map);

    auto link_penwidth_map = boost::get(&Link::penwidth, graph);
    dyn_prop.property("penwidth", link_penwidth_map);

    // �������� �����
    std::cout << "������� ��� ����� � ������ � ������� .dot:\n";
    std::string src_file_name;
    std::cin >> src_file_name;

    std::ifstream src_graph_file {src_file_name};
    try
    {
        boost::read_graphviz(src_graph_file, graph, dyn_prop);
    }
    catch (boost::directed_graph_error& exc)
    {
        std::cerr << "������ - ���� ������ ���� �����������������! "
            "����� �� ���������...";
        getch();
        return EXIT_FAILURE;
    }
    catch (boost::bad_graphviz_syntax& exc)
    {
        std::cerr << "�� ������� ��������� ����! ����� �� ���������...";
        getch();
        return EXIT_FAILURE;
    }

    // ���� ������ � ����� ����
    std::string start_vertex_name, dest_vertex_name;
    std::cout <<
        "������� ����� ������ �����, ��������������� ������ � ����� ����:\n";
    std::cin >> start_vertex_name >> dest_vertex_name;

    // ������� ����������� ��������� ������
    Vertex start, dest;
    bool start_find_success = false,
         dest_find_success = false;
    VertexIter vi, vend;
    for (std::tie(vi, vend) = boost::vertices(graph); vi != vend; ++ vi)
    {
        if (node_id_map[*vi] == start_vertex_name)
        {
            start = *vi;
            start_find_success = true;
        }
        if (node_id_map[*vi] == dest_vertex_name)
        {
            dest = *vi;
            dest_find_success = true;
        }
    }
    if ( !(start_find_success && dest_find_success) )
    {
        std::cerr << "��������� ������� �� �������! ����� �� ���������...";
        getch();
        return EXIT_FAILURE;
    }

    // �������� ������� ��������� ��������
    std::vector<Vertex> predecessors (boost::num_vertices(graph));

    // ���������� ��������� ��������
    try
    {
        boost::dijkstra_shortest_paths
        (graph, start, boost::weight_map(link_weight_map)
                             .predecessor_map(&predecessors[0]) );
    }
    catch (boost::negative_edge& exc)
    {
        std::cerr << "������ - ���� �� ����� ��������� ����� "
            "� ������������� �����! ����� �� ���������...";
        getch();
        return EXIT_FAILURE;
    }

    // ������� ����������� ����
    // ���� ����� �������� ��������� �������:
    //std::cout //<< node_id_map[*vi] << " "
                //<< node_id_map[predecessors[*vi]] << " "
                //<< node_id_map[predecessors[predecessors_v[*vi]]]

    // ��������� ������ ������������� ����
    // ���� �� ������� ���������� � ������

    std::vector<Vertex> vertices_path {};
    for (Vertex vert = dest; ; vert = predecessors[vert])
    {
        if (vert == predecessors[vert])  // ���� ��������������� �����������
            vert = start;

        node_color_map[vert] = "green";
        node_fontcolor_map[vert] = "green";
        node_penwidth_map[vert] = 3.0;
        vertices_path.push_back(vert);

        if (vert == start) break;
    }

    // ��������� �����
    EdgeIter ei, eend;
    for (std::tie(ei, eend) = boost::edges(graph); ei != eend; ++ ei)
    {
        // ������� ��� ������� ����� ������� �������
        DEVTOOLS::symmetric_pair<Vertex> incident_vertices
            {boost::incident(*ei, graph)};

        for (auto vert = vertices_path.begin();
                vert != vertices_path.end() - 1;
                ++ vert)
        {
            if ( incident_vertices.contains(*vert, *(vert + 1)) )
            {
                link_color_map[*ei] = "green";
                link_fontcolor_map[*ei] = "green";
                link_penwidth_map[*ei] = 3.0;
            }
        }
    }

    // ����� �����
    std::cout << "������� ��� ����� ��� ������ ����������:\n";
    std::string trg_file_name;
    std::cin >> trg_file_name;

    std::ofstream trg_graph_file {trg_file_name};
    boost::write_graphviz_dp
        (trg_graph_file, graph, dyn_prop, "node_id", node_id_map);
    std::cout << "���� ������� ��������! ����� �� ���������...";

    getch();
    return EXIT_SUCCESS;
}
