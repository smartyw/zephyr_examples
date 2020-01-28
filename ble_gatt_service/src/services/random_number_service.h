void random_number_service_init();
void rnd_ccc_cfg_changed(const struct bt_gatt_attr *attr, u16_t value);
ssize_t write_rnd_frequency(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags);
u8_t get_random_number_subscribed();
void start_generating_random_numbers();
void stop_generating_random_numbers();
void set_connection(struct bt_conn *conn);