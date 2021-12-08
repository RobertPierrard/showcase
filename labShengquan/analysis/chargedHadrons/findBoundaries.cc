
void findBoundaries(){
  int numBins = 100;
  double impact_param[] = {0, 1.31359, 1.8714, 2.30769, 2.67757, 2.99419, 3.28428, 3.55492, 3.80533, 4.04146, 4.26545, 4.48267, 4.68512, 4.88004, 5.07111, 5.24911, 5.43214, 5.60621, 5.77462, 5.93768, 6.09838, 6.26215, 6.41673, 6.56787, 6.71419, 6.85761, 6.99976, 7.14128, 7.28103, 7.4185, 7.55519, 7.68848, 7.82074, 7.95219, 8.08063, 8.20733, 8.33079, 8.45228, 8.57555, 8.69632, 8.81596, 8.93423, 9.05566, 9.17497, 9.29348, 9.40746, 9.52337, 9.63551, 9.74917, 9.86537, 9.97605, 10.0853, 10.1971, 10.3065, 10.4156, 10.5232, 10.6334, 10.7398, 10.8494, 10.9584, 11.0651, 11.1722, 11.2771, 11.3825, 11.4855, 11.5905, 11.6955, 11.8007, 11.9058, 12.0077, 12.1123, 12.2157, 12.3188, 12.4213, 12.5237, 12.6253, 12.7277, 12.8286, 12.9305, 13.0311, 13.1316, 13.2345, 13.3365, 13.4424, 13.5475, 13.652, 13.7586, 13.8682, 13.9806, 14.0948, 14.214, 14.3405, 14.4728, 14.6211, 14.7781, 14.9562, 15.1608, 15.4122, 15.7457, 16.2602, 20.5058};

  for(int i=0; i<numBins+1; i++){
    if((i==0) || (i==5) || (i==10) || (i==30) || (i==50) || (i==70)|| (i==90)){
      cout << impact_param[i] << ", " ;
    }
  }
  cout << endl;
}