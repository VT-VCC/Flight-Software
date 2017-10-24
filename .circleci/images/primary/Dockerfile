FROM centos:7
RUN yum install -y epel-release; yum clean all;
RUN yum install -y clang cmake3 ninja-build; yum clean all; rm -rf /var/cache/yum
# We need to install GCC because Clang relies on it under CentOS because reasons
RUN yum install -y gcc gcc-c++; yum clean all; rm -rf /var/cache/yum