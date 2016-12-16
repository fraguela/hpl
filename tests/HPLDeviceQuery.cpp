#include <iostream>
#include "HPL.h"

using namespace std;
using namespace HPL;

void displayDevice(const Device& device)
{ DeviceProperties auxDP;
  static const char *names [] = { "none", "read-only", "write-only", "read-write" };
  
  cout << "Platform " << device.getPlatformId() << " DeviceType " << device.getDeviceType() << " Device number " << device.getDeviceNumber() << endl;
  
  device.getProperties(auxDP);
  cout << "Device name.......................... " << auxDP.deviceName << endl;
  cout << "Global memory size (in bytes)........ " << auxDP.globalMemSize << "  unified=" << auxDP.unifiedMemory << endl;
  cout << "Local memory size (in bytes)......... " << auxDP.localMemSize << "  dedicated=" << auxDP.dedicatedLocalMem << endl;
  cout << "Maximum buffer size (in bytes)....... " << auxDP.maxMemAllocSize << endl;
  cout << "Global memory cache size (in bytes).. " << auxDP.globalMemCacheSize << endl;
  cout << "Global memory cache type............. " << names[(int)auxDP.globalMemCacheType] << endl;
  cout << "Compute units........................ " << auxDP.computeUnits << endl;
  cout << "Domain dimensions supported.......... " << auxDP.maxDimensions << endl;
  cout << "Local domain sizes................... " << "(" << auxDP.maxLocalDomainSizes[0] << "," << auxDP.maxLocalDomainSizes[1] << "," << auxDP.maxLocalDomainSizes[2] << ")\n";  
  cout << "Maximum size of local domain......... " << auxDP.maxLocalDomainSize << endl;
  cout << "Double precision support.............." << (device.supportsDoubles() ? 'Y' : 'N') << endl;
}

int main()
{
  for ( Device_t devt = (Device_t)0; 
        devt < MAX_DEVICE_TYPES; 
        devt = (Device_t)((int)devt + 1) ) {
    const unsigned int ndevices = getDeviceNumber(devt);
    for(unsigned int i = 0; i < ndevices; i++)
    {
      cout << "----------------------------------------------------------\n";
      cout << Device::getGenericName(devt) << " number " << i << endl;
      displayDevice(Device(devt,i));
    }
  }
  
  cout << "Test is PASSED.\n";

  return 0;
}
