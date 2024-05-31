# TheKCommonSubstringProblem
This repository includes an implementation of the algorithm of Hui [1] and two versions of the algorithm of Chrochemore et al. [2].
For the implementation the following variation by Jannik Olbrich of the sdsl-lite-library [3] is used: https://gitlab.com/qwerzuiop/sdsl-lite-gsa.

## License
This code is free software provided under the GNU General Public License (GPLv3). For more information see the [LICENSE](LICENSE.txt) file.

## Example Inputs
The files used in the examples are either DNA data from: https://www.uni-ulm.de/compressed-debruijn-graph-data/ (used are the first 4 E.coli bacteria and the first 3 versions of the human chromosome 1) and modified so they don't contain the characters 'N* and '\n' anymore or random generated strings of upper case letters. The examples will always have one version with DNA data and one with random input data with the same length.

### Example One (3 E.coli bacteria):
DNA data:
```
    std::vector<std::string> inputstrings(3);

    std::string Ecoli1;
    std::string Ecoli2;
    std::string Ecoli3;

    std::ifstream file("Ecoli1Output.fasta");
    std::getline(file, Ecoli1);
    std::ifstream file2("Ecoli2Output.fasta");
    std::getline(file2, Ecoli2);
    std::ifstream file3("Ecoli3Output.fasta");
    std::getline(file3, Ecoli3);

    inputstrings[0] = Ecoli1;
    inputstrings[1] = Ecoli2;
    inputstrings[2] = Ecoli3;

    std::vector<std::string> result;

    ChrochemoreEtAl c;
    result = c.KGTP(inputstrings);

    for(int i=0; i<result.size(); i++){
        std::cout << i+1 << ": " << result[i] << std::endl;
    }
```
Random data:
```
    std::vector<std::string> inputstrings(3);

    inputstrings.resize(3);

    std::string Random1;
    std::string Random2;
    std::string Random3;

    std::ifstream fileR("RandomEcoli1Length.txt");
    std::getline(fileR, Random1);
    std::ifstream fileR2("RandomEcoli2Length.txt");
    std::getline(fileR2, Random2);
    std::ifstream fileR3("RandomEcoli3Length.txt");
    std::getline(fileR3, Random3);

    inputstrings[0] = Random1;
    inputstrings[1] = Random2;
    inputstrings[2] = Random3;

    std::vector<std::string> result;

    ChrochemoreEtAl c;
    result = c.KGTP(inputstrings);

    for(int i=0; i<result.size(); i++){
        std::cout << i+1 << ": " << result[i] << std::endl;
    }
```

### Example Two (3 E.coli bacteria splitted in half):
DNA data:
```
    std::vector<std::string> inputstrings;

    std::string Ecoli1;
    std::string Ecoli2;
    std::string Ecoli3;

    std::ifstream file("Ecoli1Output.fasta");
    std::getline(file, Ecoli1);
    std::ifstream file2("Ecoli2Output.fasta");
    std::getline(file2, Ecoli2);
    std::ifstream file3("Ecoli3Output.fasta");
    std::getline(file3, Ecoli3);

    inputstrings.resize(6);

    inputstrings[0] = Ecoli1.substr(0, Ecoli1.size()/2);
    inputstrings[1] = Ecoli1.substr(Ecoli1.size()/2);
    inputstrings[2] = Ecoli2.substr(0, Ecoli2.size()/2);
    inputstrings[3] = Ecoli2.substr(Ecoli2.size()/2);
    inputstrings[4] = Ecoli3.substr(0, Ecoli3.size()/2);
    inputstrings[5] = Ecoli3.substr(Ecoli3.size()/2);
    std::vector<std::string> result;

    ChrochemoreEtAl c;
    result = c.KGTP(inputstrings);

    for(int i=0; i<result.size(); i++){
        std::cout << i+1 << ": " << result[i] << std::endl;
    }
```

Random data:
```
    std::vector<std::string> inputstrings;

    std::string Random1;
    std::string Random2;
    std::string Random3;

    std::ifstream fileR("RandomEcoli1Length.txt");
    std::getline(fileR, Random1);
    std::ifstream fileR2("RandomEcoli2Length.txt");
    std::getline(fileR2, Random2);
    std::ifstream fileR3("RandomEcoli3Length.txt");
    std::getline(fileR3, Random3);

    inputstrings.resize(6);

    inputstrings[0] = Random1.substr(0, Random1.size()/2);
    inputstrings[1] = Random1.substr(Random1.size()/2);
    inputstrings[2] = Random2.substr(0, Random2.size()/2);
    inputstrings[3] = Random2.substr(Random2.size()/2);
    inputstrings[4] = Random3.substr(0, Random3.size()/2);
    inputstrings[5] = Random3.substr(Random3.size()/2);

    std::vector<std::string> result;

    ChrochemoreEtAl c;
    result = c.KGTP(inputstrings);

    for(int i=0; i<result.size(); i++){
        std::cout << i+1 << ": " << result[i] << std::endl;
    }    
```

### Example Three (1 E.coli bacterium splitted up in parts of increasing length):
DNA data:
```
    std::vector<std::string> inputstrings;

    std::string Ecoli1;

    std::ifstream file("Ecoli1Output.fasta");
    std::getline(file, Ecoli1);

    int index = 0;

    std::list<string> parts;
    for(int i=50000; i<Ecoli1.size(); i+=counter*50000){
        parts.push_back(Ecoli1.substr(index, i-index));
        index =  i;
    }

    inputstrings.resize(parts.size());

    int j = 0;

    for(auto s : parts){
        inputstrings[j] = s;
        j++;
    }

    std::vector<std::string> result;

    ChrochemoreEtAl c;
    result = c.KGTP(inputstrings);

    for(int i=0; i<result.size(); i++){
        std::cout << i+1 << ": " << result[i] << std::endl;
    }
```

Random data:
```
    std::vector<std::string> inputstrings;

    std::string Random1;

    std::ifstream fileR("RandomEcoli1Length.txt");
    std::getline(fileR, Random1);

    int index = 0;

    std::list<string> partsR;
    for(int i=50000; i<Random1.size(); i+=counter*50000){
        partsR.push_back(Random1.substr(index, i-index));
        index =  i;
    }

    inputstrings.resize(partsR.size());

    int j = 0;

    for(auto s : partsR){
        inputstrings[j] = s;
        j++;
    }

    std::vector<std::string> result;

    ChrochemoreEtAl c;
    result = c.KGTP(inputstrings);

    for(int i=0; i<result.size(); i++){
        std::cout << i+1 << ": " << result[i] << std::endl;
    }
```

### Example 4 (1 E.coli bacterium splitted up in parts of length 1000):
DNA data:
```
    std::vector<std::string> inputstrings;

    std::string Ecoli1;

    std::ifstream file("Ecoli1Output.fasta");
    std::getline(file, Ecoli1);

    int index = 0;

    std::list<string> parts2;
    for(int i=1000; i<Ecoli1.size(); i+=1000){
        parts2.push_back(Ecoli1.substr(i-1000, 1000));
        index = i;
    }

    parts2.push_back(Ecoli1.substr(index, Ecoli1.size()-index));    

    inputstrings.resize(parts2.size());

    int j = 0;

    for(auto s : parts2){
        inputstrings[j] = s;
        j++;
    }

    std::vector<std::string> result;

    ChrochemoreEtAl c;
    result = c.KGTP(inputstrings);

    for(int i=0; i<result.size(); i++){
        std::cout << i+1 << ": " << result[i] << std::endl;
    }
```

Random data:
```
    std::vector<std::string> inputstrings;

    std::string Random1;

    std::ifstream fileR("RandomEcoli1Length.txt");
    std::getline(fileR, Random1);

    int index = 0;

    std::list<string> partsR2;
    for(int i=1000; i<Random1.size(); i+=1000){
        partsR2.push_back(Random1.substr(i-1000, 1000));
        index = i;
    }

    partsR2.push_back(Random1.substr(index, Random1.size()-index));    

    inputstrings.resize(partsR2.size());

    int j = 0;

    for(auto s : partsR2){
        inputstrings[j] = s;
        j++;
    }

    std::vector<std::string> result;

    ChrochemoreEtAl c;
    result = c.KGTP(inputstrings);

    for(int i=0; i<result.size(); i++){
        std::cout << i+1 << ": " << result[i].size() << std::endl;
    }
```

### Example Five (3 versions of the human chromosome 1):
DNA data:
```
    std::vector<std::string> inputstrings(3);

    std::string Chromosom1;
    std::string Chromosom2;
    std::string Chromosom3;

    std::ifstream file("Chromosom1Output.fa");
    std::getline(file, Chromosom1);
    std::ifstream file2("Chromosom2Output.fa");
    std::getline(file2, Chromosom2);
    std::ifstream file3("Chromosom3Output.fa");
    std::getline(file3, Chromosom3);

    inputstrings[0] = Chromosom1;
    inputstrings[1] = Chromosom2;
    inputstrings[2] = Chromosom3;

    std::vector<std::string> result;

    ChrochemoreEtAl c;

    result = c.KGTP(inputstrings);

    for(int j=0; j<result.size(); j++){
        std::cout << j << ": " << result[j] << std::endl;
    }
```

Random data:
```
    std::vector<std::string> inputstrings(3);

    std::string Random1;
    std::string Random2;
    std::string Random3;

    std::ifstream file("RandomChromosom1Length.txt");
    std::getline(file, Random1);
    std::ifstream file2("RandomChromosom2Length.txt");
    std::getline(file2, Random2);
    std::ifstream file3("RandomChromosom3Length.txt");
    std::getline(file3, Random3);

    inputstrings[0] = Random1;
    inputstrings[1] = Random2;
    inputstrings[2] = Random3;

    std::vector<std::string> result;

    ChrochemoreEtAl c;

    result = c.KGTP(inputstrings);

    for(int j=0; j<result.size(); j++){
        std::cout << j << ": " << result[j] << std::endl;
    }
```

# Bibliography
[1] L.C.K. Hui. „Color Set Size Problem with Applications to String Matching“. In: Combinatorial Pattern Matching. Springer Berlin Heidelberg, 1992, S. 230–243.  
[2] M. Crochemore et al. „The longest common substring problem“. In: Mathematical Structures in Computer Science (2015).  
[3] S. Gog et al. „From Theory to Practice: Plug and Play with Succinct Data Structures“. In: 13th International Symposium on Experimental Algorithms, (SEA 2014). 2014, S. 326–337.  
