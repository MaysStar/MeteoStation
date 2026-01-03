FROM php:8.2-fpm

WORKDIR /var/www/html

# Встановлюємо Nginx
RUN apt-get update && apt-get install -y nginx \
    && rm -rf /var/lib/apt/lists/*

# Копіюємо конфігурації
COPY default.conf /etc/nginx/sites-available/default
RUN ln -sf /etc/nginx/sites-available/default /etc/nginx/sites-enabled/

# Копіюємо PHP файл
COPY index.php /var/www/html/

# Налаштовуємо права
RUN chmod -R 755 /var/www/html && \
    mkdir -p /var/run/php

# Запускаємо обидва сервіси через bash
CMD bash -c "php-fpm -D && nginx -g 'daemon off;'"

EXPOSE 8080