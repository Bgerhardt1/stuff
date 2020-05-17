FROM electronbuilds/libchromiumcontent:0.0.4

USER root

# Set up HOME directory
ENV HOME=/home
RUN chmod a+rwx /home

# Install node.js
RUN curl -sL https://deb.nodesource.com/setup_10.x | bash -
RUN apt-get install -y nodejs

# Install wget used by crash reporter
RUN apt-get install -y wget

# Install python-dbusmock
RUN apt-get install -y python-dbusmock

# Install libnotify
RUN apt-get install -y libnotify-bin

# Add xvfb init script
ADD tools/xvfb-init.sh /etc/init.d/xvfb
RUN chmod a+x /etc/init.d/xvfb
