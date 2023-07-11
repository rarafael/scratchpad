#include <chrono>
#include <iostream>
#include <thread>

#include <libtorrent/alert_types.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [MAGNET-URI]" << std::endl;
        return 1;
    }

    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask,
                     lt::alert_category::status | lt::alert_category::error);

    for (int i = 1; i < argc; i++) {
        try {
            lt::session sess(settings);
            lt::add_torrent_params pars;
            pars = lt::parse_magnet_uri(argv[i]);
            pars.save_path = ".";
            lt::torrent_handle hand = sess.add_torrent(pars);

            std::cout << "Starting download of: \'" << pars.name << "\'..." << std::endl;

            bool is_done = false;
            while (!is_done) {
                std::vector<lt::alert*> alert;
                sess.pop_alerts(&alert);

                lt::torrent_status status = hand.status();

                std::int64_t sess_size = status.total;
                std::string sess_sizestr;
                if (sess_size > (1000 * 1000)) {
                    sess_size /= (1000 * 1000);
                    sess_sizestr = "MB at ";
                } else {
                    sess_size /= (1000);
                    sess_sizestr = "KB at ";
                }

                std::cout << status.name << ": " <<
                sess_size << sess_sizestr <<
                status.download_payload_rate / 1000 << "KB/s [" <<
                status.progress_ppm / 10000 << "%]" <<
                std::endl;

                for (lt::alert const *al : alert) {
                    if (lt::alert_cast<lt::torrent_finished_alert>(al) ||
                        lt::alert_cast<lt::torrent_error_alert>(al))
                        is_done = true;
                }

                sess.post_torrent_updates();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            std::cout << "Finished \'" << pars.name << "\'" << std::endl;
        } catch(std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0; /* SUCCESS */
}
