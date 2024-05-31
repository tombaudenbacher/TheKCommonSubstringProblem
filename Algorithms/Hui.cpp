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
#include <sys/resource.h>
#include <sys/time.h>


using namespace sdsl;
using namespace std;

using cst_t = sdsl::cst_sada<sdsl::csa_bitcompressed<>, sdsl::lcp_bitcompressed<>>;
using node_t = sdsl::int_vector_size_type;

class Hui{

    std::vector<int> css;
    std::vector<int> stringlänge;
    std::vector<int> farbe;
    std::vector<int> blattAnzahl;
    std::vector<int> vorhergehendesBlatt;
    std::vector<int> gWert;
    std::vector<int> GWert;
    std::list<int> blattListe;
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
        //+1, weil das implizite \0 am Ende relevant für einen Knoten ist
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

        //Array teilstring initialisieren
        //+2 um für den Index-Shift und den extra Knoten von \0 zu kompensieren
        teilstring.resize(C.size()+2, -1);

        //Suffix-Baum erzeugen
        cst_t cst;
	    sdsl::construct_im(cst, text, 1, sdsl::TEXT_COLLECTION);

        css.resize(cst.nodes(), -1);
        farbe.resize(cst.nodes(), -1);
        stringlänge.resize(cst.nodes(), -1);
        blattAnzahl.resize(cst.nodes(), -1);
        vorhergehendesBlatt.resize(cst.nodes(), -1);
        gWert.resize(cst.nodes(), 0);
        GWert.resize(cst.nodes(), -1);


        //CSSP lösen und direkt die Knoten für die längsten Teilstrings berechnen
        blattAnzahlUndBlattListe(cst, cst.root());
        vorhergehendeBlätter(cst, C.size());
        gWerte(cst);
        cssUndGWerte(cst, cst.root());

        //genau -> mindestens
        //Es muss aufgrund des extra Knoten zusätzlich noch 1 von size() abgezogen werden
        for(int i=teilstring.size()-3; i>=1; --i){
            if(stringlänge[teilstring[i+1]]>stringlänge[teilstring[i]]){
                teilstring[i] = teilstring[i+1];
            }
        }

        //Ausgabestrings suchen
        std::vector<std::string> output(C.size());
        //-1, aufgrund des extra Knoten
        //teilstring[i] kann -1 sein, da der Wurzelknoten, aufgrund des extra Knoten einen um 1 höheren css-Wert hat, als er haben sollte
        for(int i=1; i<teilstring.size()-1; i++){
            if(teilstring[i] == -1){
                output[i-1] = "";
            }else{
                output[i-1] = labelAuslesen(cst, cst.inv_id(teilstring[i]));
            }
        }
        return output;
    }

    int blattAnzahlUndBlattListe(cst_t& cst, node_t u){
        if(cst.is_leaf(u)){
            blattListe.push_back(cst.id(u));
			return 1;
		}
        blattAnzahl[cst.id(u)] = 0;
		for(auto v : cst.children(u)){
            blattAnzahl[cst.id(u)] += blattAnzahlUndBlattListe(cst, v);
        }
        return blattAnzahl[cst.id(u)];
	}

    void vorhergehendeBlätter(cst_t& cst, int m){
        std::vector<int> letztes(m+1, -1);
        for(auto l : blattListe){
            farbeBerechnen(cst, cst.inv_id(l));
            vorhergehendesBlatt[l] = letztes[farbe[l]];
            letztes[farbe[l]] = l;
        }
    }

    void gWerte(cst_t& cst){
        for(auto l : blattListe){
            if(vorhergehendesBlatt[l] == -1){
                continue;
            }
            node_t u = cst.lca(cst.inv_id(vorhergehendesBlatt[l]), cst.inv_id(l));
            gWert[cst.id(u)] += 1;
        }
    }

    int cssUndGWerte(cst_t& cst, node_t u){
        if(cst.is_leaf(u)){
            stringlänge[cst.id(u)] = tiefeEinesBlattes(cst, u);
            if(teilstring[1] == -1 || stringlänge[cst.id(u)] > stringlänge[teilstring[1]]){
                teilstring[1] = cst.id(u);
            }
			return 0;
		}
        stringlänge[cst.id(u)] = cst.depth(u);
        GWert[cst.id(u)] = gWert[cst.id(u)];
		for(auto v : cst.children(u)){
            int childValue = cssUndGWerte(cst, v);
            GWert[cst.id(u)] = GWert[cst.id(u)] + childValue;
        }
        css[cst.id(u)] = blattAnzahl[cst.id(u)] - GWert[cst.id(u)];
        if(teilstring[css[cst.id(u)]] == -1 || stringlänge[cst.id(u)] > stringlänge[teilstring[css[cst.id(u)]]]){
            teilstring[css[cst.id(u)]] = cst.id(u);
        }
        return GWert[cst.id(u)];
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
};
