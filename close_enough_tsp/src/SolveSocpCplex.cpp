#include "close_enough_tsp/SolveSocpCplex.h"

ILOSTLBEGIN // import namespace std

SolveSocpCplex::SolveSocpCplex(Data *dataObject, vector<int> sequence)
        : model(env), SOCP(model), xCoord(env), yCoord(env), zCoord(env),
          x(env, sequence.size(), -IloInfinity, IloInfinity), y(env, sequence.size(), -IloInfinity, IloInfinity),
          z(env, sequence.size(), -IloInfinity, IloInfinity), objectData(dataObject) {
    setSizeProblem(sequence);
    f_value = 0;
}

// destrutor
SolveSocpCplex::~SolveSocpCplex() {
    env.end();
}

int SolveSocpCplex::setSizeProblem(vector<int> sequence) {
    sizeProblem = sequence.size();
    return sizeProblem;
}

void SolveSocpCplex::setF_value() {
    f_value = SOCP.getObjValue();
}

double SolveSocpCplex::getF_value() {
    return f_value;
}

void SolveSocpCplex::printF_value() {
    cout << "Value of objective function: " << f_value << endl;
}

void SolveSocpCplex::printSolution([[maybe_unused]] vector<int> sequence) {
    vector<IloNumArray> solution;

    solution.push_back(xCoord);
    solution.push_back(yCoord);
    solution.push_back(zCoord);

    cout << "Solution: " << endl;

    for (int j = 0; j < sizeProblem; j++) {
        cout << "( " << solution[0][j] << ", " << solution[1][j] << ", " << solution[2][j] << " )" << endl;
    }

    cout << endl;
}

double SolveSocpCplex::getSolutionX(int i) {
    return xCoord[i];
}

double SolveSocpCplex::getSolutionY(int i) {
    return yCoord[i];
}

double SolveSocpCplex::getSolutionZ(int i) {
    return zCoord[i];
}

void SolveSocpCplex::solveSOCP(vector<int> sequence) {

    static pthread_mutex_t cs_mutex = PTHREAD_MUTEX_INITIALIZER;

    createModel(sequence);
    SOCP.setOut(env.getNullStream());
    SOCP.setParam(IloCplex::BarQCPEpComp, 1e-12);
    SOCP.setParam(IloCplex::Threads, 1);
    SOCP.setParam(IloCplex::ParallelMode, 1);
    SOCP.solve();
    SOCP.getStatus();
    violation = SOCP.getQuality(IloCplex::SumScaledPrimalInfeas);
    setF_value();
    SOCP.getValues(xCoord, x);
    SOCP.getValues(yCoord, y);
    SOCP.getValues(zCoord, z);

    pthread_mutex_unlock(&cs_mutex);
}

void SolveSocpCplex::finishSOCP() {
    SOCP.end();
}

void SolveSocpCplex::createModel(vector<int> sequence) {
    //variables
    //add variable f ( head of cones )
    //================================================================
    IloNumVarArray f(env, sizeProblem, 0, IloInfinity);
    char var[100];
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "f(%d)", i);
        f[i].setName(var);
        model.add(f[i]);
    }
    //getchar();
    //add variables corresponding to coordinates x, y and z
    //================================================================
    //variable x
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "x(%d)", i);
        x[i].setName(var);
        model.add(x[i]);
    }

    //variable y
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "y(%d)", i);
        y[i].setName(var);
        model.add(y[i]);
    }

    //variable z
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "z(%d)", i);
        z[i].setName(var);
        model.add(z[i]);
    }

    //add auxiliary variables w, u, v, s, t and q
    //================================================================
    IloNumVarArray w(env, sizeProblem, -IloInfinity, IloInfinity);
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "w(%d)", i);
        w[i].setName(var);
        model.add(w[i]);
    }

    IloNumVarArray u(env, sizeProblem, -IloInfinity, IloInfinity);
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "u(%d)", i);
        u[i].setName(var);
        model.add(u[i]);
    }

    IloNumVarArray v(env, sizeProblem, -IloInfinity, IloInfinity);
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "v(%d)", i);
        v[i].setName(var);
        model.add(v[i]);
    }

    IloNumVarArray s(env, sizeProblem, -IloInfinity, IloInfinity);
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "s(%d)", i);
        s[i].setName(var);
        model.add(s[i]);
    }

    IloNumVarArray t(env, sizeProblem, -IloInfinity, IloInfinity);
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "t(%d)", i);
        t[i].setName(var);
        model.add(t[i]);
    }

    IloNumVarArray q(env, sizeProblem, -IloInfinity, IloInfinity);
    for (int i = 0; i < sizeProblem; i++) {
        sprintf(var, "q(%d)", i);
        q[i].setName(var);
        model.add(q[i]);
    }

    //Set objetive function
    //=============================================
    IloExpr FO(env);
    for (int i = 0; i < sizeProblem; i++) {
        FO += f[i];
    }

    model.add(IloMinimize(env, FO));

    //set SOC constraints
    //===============================================
    for (int j = 0; j < sizeProblem; j++) {
        IloRange r = (-f[j] * f[j] + w[j] * w[j] + u[j] * u[j] + v[j] * v[j] <= 0);
        char c[100];
        sprintf(c, "cone%d", j);
        r.setName(c);
        model.add(r);
    }

    for (int i = 0; i < sizeProblem; i++) {
        IloRange r = (s[i] * s[i] + t[i] * t[i] + q[i] * q[i] <=
                      objectData->getRadius(sequence[i]) * objectData->getRadius(sequence[i]));
        char c[100];
        sprintf(c, "q%d", i);
        r.setName(c);
        model.add(r);
    }

    //radius constraints
    //=============================================================
    for (int i = 0; i < sizeProblem; i++) {
        IloRange r = (s[i] + x[i] == objectData->getCoordx(sequence[i]));
        char c[100];
        sprintf(c, "b%d", i);
        r.setName(c);
        model.add(r);
    }

    for (int i = 0; i < sizeProblem; i++) {
        IloRange r = (t[i] + y[i] == objectData->getCoordy(sequence[i]));
        char c[100];
        sprintf(c, "b%d", i + sizeProblem);
        r.setName(c);
        model.add(r);
    }

    for (int i = 0; i < sizeProblem; i++) {
        IloRange r = (q[i] + z[i] == objectData->getCoordz(sequence[i]));
        char c[100];
        sprintf(c, "b%d", i + 2 * sizeProblem);
        r.setName(c);
        model.add(r);
    }

    //#########################################################################################################################
    //separating first constraint
    IloRange isol_1 = (w[0] - x[sizeProblem - 1] + x[0] == 0);
    char c[100];
    sprintf(c, "c%d", 0);
    isol_1.setName(c);
    model.add(isol_1);

    for (int j = 1; j < sizeProblem; j++) {
        IloRange r = (w[j] - x[j - 1] + x[j] == 0);
        char c[100];
        sprintf(c, "c%d", j);
        r.setName(c);
        model.add(r);
    }

    IloRange isol_2 = (u[0] - y[sizeProblem - 1] + y[0] == 0);
    char c2[100];
    sprintf(c2, "c%d", sizeProblem);
    isol_2.setName(c2);
    model.add(isol_2);

    for (int j = 1; j < sizeProblem; j++) {
        IloRange r = (u[j] - y[j - 1] + y[j] == 0);
        char c[100];
        sprintf(c, "c%d", j + sizeProblem);
        r.setName(c);
        model.add(r);
    }

    IloRange isol_3 = (v[0] - z[sizeProblem - 1] + z[0] == 0);
    char c3[100];
    sprintf(c3, "c%d", sizeProblem);
    isol_3.setName(c3);
    model.add(isol_3);

    for (int j = 1; j < sizeProblem; j++) {
        IloRange r = (v[j] - z[j - 1] + z[j] == 0);
        char c[100];
        sprintf(c, "c%d", j + 2 * sizeProblem);
        r.setName(c);
        model.add(r);
    }
}





















