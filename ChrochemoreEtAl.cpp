#include <sdsl/config.hpp>
#include <sdsl/construct.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <list>


using namespace sdsl;
using namespace std;

using cst_t = sdsl::cst_sada<sdsl::csa_bitcompressed<>, sdsl::lcp_bitcompressed<>>;
using node_t = sdsl::int_vector_size_type;

class ChrochemoreEtAl{

    std::vector<int> css;
    std::vector<int> stringlänge;
    std::unordered_map<int, int> repräsentant;
    std::vector<int> farbe;
    std::vector<int> anzahl;
    std::vector<int> vorhergehendesBlatt;
    std::vector<int> teilstring;
    std::vector<int> A;
    std::vector<int> indexe;

    public:
    std::vector<std::string> KGTP(std::vector<std::string>& C){
        //Eingabestrings auf Suffix-Baum-Konstruktion vorbereiten
        string input = "";
        for(int i=0; i<C.size(); i++){
            input += C[i] + "\1";
        }

        //Array für Bestimmung des Eingabestrings
        //+1, weil das \0 am Ende relevant für einen Knoten ist
        indexe.resize(C.size()+1);
        indexe[C.size()] = 0;
        indexe[C.size()-1] = 1;
        for(int i=C.size()-2; i>=0; --i){
            indexe[i] = indexe[i+1] + C[i+1].size() + 1;
        }

        const char* inputChr = input.c_str();
        sdsl::int_vector<0> text(strlen(inputChr), 0, 8);
        for (size_t i = 0; i < text.size(); i++) {
            text[i] = input[i];
        }

        //Arrays vorhergehendesBlatt und teilstring initialisieren
        //+2 um für den Index-Shift und den extra Knoten von \0 zu kompensieren
        vorhergehendesBlatt.resize(C.size()+1, -1);
        teilstring.resize(C.size()+2, -1);

        //Suffix-Baum erzeugen
        cst_t cst;
	    sdsl::construct_im(cst, text, 1, sdsl::TEXT_COLLECTION);

        css.resize(cst.nodes(), -1);
        farbe.resize(cst.nodes(), -1);
        stringlänge.resize(cst.nodes(), -1);

        //Union-Find-Array initialisieren
        ufInitialisieren(cst);

        //CSSP lösen und direkt die Knoten für die längsten Teilstrings berechnen#
        cssWerte(cst, cst.root());

        //genau -> mindestens
        //Es muss aufgrund des extra Knoten zusätzlich noch 1 von size() abgezogen werden
        for(int i=teilstring.size()-2; i>=1; --i){
            if(stringlänge[teilstring[i+1]]>stringlänge[teilstring[i]]){
                teilstring[i] = teilstring[i+1];
            }
        }
        //Ausgabestrings suchen
        std::vector<std::string> output(C.size());
        //-1, aufgrund des extra Knoten
        //teilstring[i] kann -1 sein, da der Wurzelknoten, aufgrund des extra Knoten einen um 1 höheren css-Wert hat, als er haben sollte
        for(int i=1; i<teilstring.size()-1; i++){
            output[i-1] = labelAuslesen(cst, cst.inv_id(teilstring[i]));
        }
        return output;
    }

    void cssWerte(cst_t& cst, node_t u){
        makeSet(cst.id(u));
        css[cst.id(u)] = 0;
        if(cst.is_leaf(u)){
            farbeBerechnen(cst, u);
            stringlänge[cst.id(u)] = tiefeEinesBlattes(cst, u);
            int colorOfLeaf = farbe[cst.id(u)];
            int vorfahre = findRepresentative(vorhergehendesBlatt[colorOfLeaf]);
            vorhergehendesBlatt[colorOfLeaf] = cst.id(u);
            if(vorfahre != -1){
                css[vorfahre] = css[vorfahre] - 1;
            }
            css[cst.id(u)] = 1;
        } else {
            stringlänge[cst.id(u)] = cst.depth(u);
            for(auto v : cst.children(u)){
                cssWerte(cst, v);
                unionSets(find(cst.id(u)), find(cst.id(v)));
                css[cst.id(u)] = css[cst.id(u)] + css[cst.id(v)];
            }
        }
        if(teilstring[css[cst.id(u)]] == -1 || stringlänge[cst.id(u)] > stringlänge[teilstring[css[cst.id(u)]]]){
            teilstring[css[cst.id(u)]] = cst.id(u);
        }
    }

    int sucheDesEingabestrings(int tiefe){
        int string = -1;
        int links = 0;
        int rechts = indexe.size()-1;
        int mitte;

        while(links<rechts){
            mitte = (links+rechts)/2;
            if(indexe[mitte] < tiefe){
                string = mitte;
                rechts = mitte;
            }else{
                links = mitte + 1;
            }
        }
        return string;
    }

    //Gibt die Farbe des Knoten u aus cst zurück
    void farbeBerechnen(cst_t& cst, node_t u){
        int tiefe = cst.depth(u);
        farbe[cst.id(u)] = sucheDesEingabestrings(tiefe);
    }
    
    //Gibt das Label des Knoten u aus cst zurück
    string labelAuslesen(cst_t& cst, node_t u){
        string label = "";
        for(int i=1; i<=stringlänge[cst.id(u)]; i++){
            label.push_back(cst.edge(u,i));
		}
        return label;
    }

    //Gibt die Stringlänge des Substring, den der Knoten u aus cst repräsentiert, zurück
    int tiefeEinesBlattes(cst_t& cst, node_t u){
        int blattFarbe = farbe[cst.id(u)];
        int tiefe = (cst.depth(u) - 1) - indexe[blattFarbe];
        return tiefe;
    }


    //Union-Find-Datenstruktur
    void ufInitialisieren(cst_t& cst){
        A.resize(cst.nodes());
        anzahl.resize(cst.nodes());
        for (int i = 0; i < A.size(); i++) {
            A[i] = -1;
            anzahl[i] = -1;
        }
        repräsentant[-1] = -1;
    }

    void makeSet(int x){
        A[x] = x;
        repräsentant[x] = x;
        anzahl[x] = 1;
    }

    int find(int x) {
        if (x == -1) {
            return -1;
        }
        if (A[x] != x) {
            A[x] = find(A[x]);
        }
        return A[x];
    }

    int findRepresentative(int x){
        int rep = find(x); 
        return repräsentant[rep];
    }

    void unionSets(int x, int y) {
        if (x != y) {
            if (anzahl[x] < anzahl[y]) {
                A[x] = y;
                anzahl[y] = anzahl[x] + anzahl[y];
                repräsentant[y] = repräsentant[x];
            } else {
                A[y] = x;
                anzahl[x] = anzahl[x] + anzahl[y];
            }
        }
    }

};
